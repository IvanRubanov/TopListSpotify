/*
 Copyright (c) 2012, Spotify AB
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 * Neither the name of Spotify AB nor the names of its contributors may
 be used to endorse or promote products derived from this software
 without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL SPOTIFY AB BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * Tasks that can be added to the queue of tasks running on the libspotify thread
 */
#include "tasks.h"
#include "run_loop.h"
#include "jni_glue.h"
#include "logger.h"

void login(list<int> int_params, list<string> string_params, sp_session *session, sp_track *track) {
    if (session == NULL)
        exitl("Logged in before session was initialized");
    string username = string_params.front();
    string password = string_params.back();
    sp_session_login(session, username.c_str(), password.c_str(), true, NULL);
}

void on_logged_in(list<int> int_params, list<string> string_params, sp_session *session, sp_track *track) {
    sp_error error = (sp_error)int_params.front();
    bool success = (SP_ERROR_OK == error) ? true : false;

    JNIEnv *env;
    jclass cls = find_class_from_native_thread(&env);
    jmethodID methodId = env->GetStaticMethodID(cls, "onLogin", "(ZLjava/lang/String;)V");
    env->CallStaticVoidMethod(cls, methodId, success, env->NewStringUTF(sp_error_message(error)));
    env->DeleteLocalRef(cls);
}

static void got_toplist(sp_toplistbrowse *result, void *userdata) {
    jobjectArray ret;
    JNIEnv *env;
    jclass cls = find_class_from_native_thread(&env);
    ret= (jobjectArray)env->NewObjectArray(sp_toplistbrowse_num_tracks(result),
            env->FindClass("java/lang/String"),env->NewStringUTF(""));

    for(int i = 0; i < sp_toplistbrowse_num_tracks(result); i++) {
        jstring track = env->NewStringUTF(sp_track_name(sp_toplistbrowse_track(result, i)));
        env->SetObjectArrayElement(ret, i, track);
        env->DeleteLocalRef(track);
    }
    
    jmethodID methodId = env->GetStaticMethodID(cls, "onGotTopList", "([Ljava/lang/String;)V");
    env->CallStaticVoidMethod(cls, methodId, ret);
    env->DeleteLocalRef(ret);
    env->DeleteLocalRef(cls);
    sp_toplistbrowse_release(result);
}

void get_top(list<int> int_params, list<string> string_params, sp_session *session, sp_track *track) {
    string country = string_params.front();
    const char* country_code = country.c_str();
    if((unsigned)strlen(country_code) != 2) {
        exitl("Country code incorrect");
    }
    sp_toplistregion region;
    region = static_cast<sp_toplistregion>(SP_TOPLIST_REGION(country_code[0], country_code[1]));
    sp_toplistbrowse_create(session, SP_TOPLIST_TYPE_TRACKS, 
        region, NULL, got_toplist, NULL);
}
