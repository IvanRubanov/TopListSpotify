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
 * The main runloop handling libspotify events and posting tasks to libspotify.
 * There is a queue of tasks that will be processed in order. The loop will sleep
 * when there is no work to do.
 */
#include <pthread.h>
#include <errno.h>
#include <list>
#include <string>

#include "run_loop.h"
#include "logger.h"
#include "tasks.h"

using namespace std;

const uint8_t g_appkey[] = {
    // Example key, put your own key here
    0x01, 0x7A, 0xA5, 0xE0, 0xA1, 0xA4, 0x28, 0x57, 0xA8, 0x62, 0xAE, 0xA8, 0xE2, 0x71, 0xB4, 0xAB,
    0xCA, 0x77, 0x93, 0xB4, 0xB7, 0x85, 0xA5, 0x62, 0xDE, 0xA3, 0xD5, 0x8F, 0x30, 0xBF, 0xEE, 0x46,
    0x56, 0xC3, 0xEC, 0x0D, 0x78, 0x69, 0x9F, 0x51, 0x71, 0xE8, 0x32, 0x46, 0xC8, 0xF2, 0xD3, 0x17,
    0x72, 0xB8, 0xD4, 0xAE, 0xE0, 0xA2, 0x3C, 0x72, 0x9F, 0x50, 0x3C, 0xA4, 0x66, 0x41, 0x5A, 0xBB,
    0x4A, 0xF2, 0xEC, 0x61, 0xCA, 0xF7, 0xF8, 0x42, 0x0D, 0x9C, 0xFD, 0x04, 0x87, 0xB3, 0xED, 0xD0,
    0xD2, 0xF9, 0x6C, 0xEB, 0x5D, 0xFC, 0x15, 0x5B, 0x10, 0xF7, 0x45, 0x74, 0x8F, 0xE3, 0x59, 0x37,
    0x0F, 0xEB, 0xDD, 0x18, 0x1F, 0xBA, 0x19, 0xC4, 0x32, 0xA8, 0x28, 0x89, 0x26, 0x7A, 0xCA, 0x84,
    0x39, 0x20, 0x62, 0x1B, 0x39, 0xCB, 0xEE, 0xAE, 0x25, 0xE6, 0x86, 0xE6, 0x90, 0x5B, 0x39, 0x2D,
    0x4F, 0x22, 0x7A, 0x7A, 0xAC, 0xCA, 0xE5, 0xC7, 0x5C, 0x54, 0xB7, 0xD3, 0x3A, 0x54, 0xE3, 0x68,
    0x19, 0x4B, 0x86, 0x84, 0x73, 0x3E, 0x30, 0x14, 0xB5, 0xDF, 0x34, 0xEB, 0x84, 0x91, 0x0D, 0xDB,
    0xDF, 0x0D, 0x59, 0xD1, 0x0C, 0x64, 0x91, 0x4C, 0x13, 0x4A, 0x36, 0x02, 0xA6, 0xA9, 0x42, 0xE5,
    0x97, 0x7D, 0xDB, 0xFF, 0x6A, 0xFF, 0x89, 0x01, 0x86, 0x84, 0xF1, 0xA9, 0x78, 0x31, 0x51, 0x9C,
    0x94, 0x25, 0xD2, 0x21, 0x9D, 0xFE, 0xBB, 0xE3, 0x09, 0x5C, 0xBA, 0x90, 0xD0, 0x82, 0x20, 0xEE,
    0xD7, 0xF9, 0xFB, 0xC2, 0x11, 0xA2, 0x04, 0x6E, 0xD0, 0x5A, 0x73, 0xAA, 0x5B, 0x21, 0x31, 0xE0,
    0x43, 0x64, 0xD8, 0x96, 0xAB, 0x0E, 0x9D, 0xC4, 0x9C, 0xBF, 0x72, 0xA5, 0x6C, 0xE0, 0x03, 0x80,
    0xB4, 0x00, 0xE4, 0xB4, 0x68, 0x81, 0x1D, 0xC5, 0x9B, 0x01, 0x99, 0x02, 0x66, 0xCD, 0x69, 0x35,
    0xDC, 0x40, 0xA3, 0x64, 0x7F, 0x27, 0x33, 0xCB, 0xF8, 0xAF, 0x2E, 0xEB, 0x8D, 0x0B, 0xC3, 0xFA,
    0xB4, 0x06, 0x86, 0x69, 0x3E, 0xE4, 0x16, 0x49, 0x4D, 0xE9, 0x52, 0x59, 0x6B, 0x46, 0x57, 0xF5,
    0xD8, 0xB1, 0x99, 0xB9, 0xC9, 0xBA, 0x52, 0x19, 0x14, 0xF3, 0x88, 0x2E, 0x29, 0xE8, 0x36, 0xE6,
    0x8D, 0xD2, 0x34, 0x54, 0x2B, 0x09, 0x50, 0xB2, 0x0F, 0xA0, 0x8C, 0x5C, 0xF8, 0x7E, 0x0D, 0x5A,
    0x7F,
};
const size_t g_appkey_size = sizeof(g_appkey);

// Defined in the sound_driver to keep the buffer logic together
int music_delivery(sp_session *sess, const sp_audioformat *format, const void *frames, int num_frames);

// There is always only one track that can be played/paused
static sp_track *s_track = NULL;

// A task contains the function, parameters and a task name
struct Task {
    task_fptr fptr;
    list<int> int_params;
    list<string> string_params;
    string name;

    Task(task_fptr _fptr, string _name, list<int> _int_params, list<string> _string_params) :
            fptr(_fptr), name(_name), int_params(_int_params), string_params(_string_params) {
    }
};

// The queue of tasks, thread safeness is important!
static list<Task> s_tasks;
// Mutex and condition for tasks and processing of tasks
static pthread_mutex_t s_notify_mutex;
static pthread_cond_t s_notify_cond;

// Keep track of next time sp_session_process_events should run
static int s_next_timeout = 0;

void set_track(sp_track *track) {
    s_track = track;
}

void addTask(task_fptr fptr, string name, list<int> int_params, list<string> string_params) {
    log("Add task <%s> to the queue", name.c_str());
    pthread_mutex_lock(&s_notify_mutex);
    Task task(fptr, name, int_params, string_params);
    s_tasks.push_back(task);
    pthread_cond_signal(&s_notify_cond);
    pthread_mutex_unlock(&s_notify_mutex);
}

void addTask(task_fptr fptr, string name, list<string> string_params) {
    list<int> int_params;
    addTask(fptr, name, int_params, string_params);
}

void addTask(task_fptr fptr, string name, list<int> int_params) {
    list<string> string_params;
    addTask(fptr, name, int_params, string_params);
}

void addTask(task_fptr fptr, string name) {
    list<int> int_params;
    list<string> string_params;
    addTask(fptr, name, int_params, string_params);
}

static void connection_error(sp_session *session, sp_error error) {
    log("------------- Connection error: %s\n -------------", sp_error_message(error));
}
static void logged_out(sp_session *session) { log("Logged out"); }
static void log_message(sp_session *session, const char *data) {
    log("************* Message: %s *************", data);
}

static void logged_in(sp_session *session, sp_error error) {
    list<int> int_params;
    int_params.push_back(error);

    addTask(on_logged_in, "login_callback", int_params);
}

static void process_events(list<int> int_params, list<string> string_params, sp_session *session, sp_track *track) {
    do {
        sp_session_process_events(session, &s_next_timeout);
    } while (s_next_timeout == 0);
}

// run process_events on the libspotify thread
static void notify_main_thread(sp_session *session) {
    addTask(process_events, "Notify main thread: process_events");
}

static sp_session_callbacks callbacks = {
    &logged_in,
    &logged_out,
    NULL,
    &connection_error,
    NULL,
    &notify_main_thread,
    NULL,
    NULL,
    &log_message,
    NULL
};

// The main loop takes care of executing tasks on the libspotify thread.
static void libspotify_loop(sp_session *session) {

    while (true) {
        pthread_mutex_lock(&s_notify_mutex);

        // If no tasks then sleep until there is, or a timeout happens
        if (s_tasks.size() == 0) {
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            struct timespec timer_start = ts;
            ts.tv_sec += s_next_timeout / 1000;
            ts.tv_nsec += (s_next_timeout % 1000) * 1000000;

            log("Wait for new task or until %d ms", s_next_timeout);
            int reason = pthread_cond_timedwait(&s_notify_cond, &s_notify_mutex, &ts);
            // If timeout then process_events should be added to the queue
            if (reason == ETIMEDOUT) {
                pthread_mutex_unlock(&s_notify_mutex);
                addTask(process_events, "Timeout: process events");
                pthread_mutex_lock(&s_notify_mutex);
            } else { // calculate a new timeout (assuming the tasks below takes 0 time)
                struct timespec timer_end;
                clock_gettime(CLOCK_REALTIME, &timer_end);
                int delta = 0;
                delta += (timer_end.tv_sec - timer_start.tv_sec) * 1000;
                delta += (timer_end.tv_nsec - timer_start.tv_nsec) / 1000000;
                s_next_timeout -= delta;
            }
        }
        // create a copy of the list of operations since other thread can manipulate it and clear the real one
        list<Task> tasks_copy = s_tasks;
        s_tasks.clear();
        pthread_mutex_unlock(&s_notify_mutex);

        for (list<Task>::iterator it = tasks_copy.begin(); it != tasks_copy.end(); it++) {
            Task task = (*it);
            log("Running task: %s", task.name.c_str());
            task.fptr(task.int_params, task.string_params, session, s_track);
        }
    }
}

void* start_spotify(void *storage_path) {
    string path = (char *)storage_path;

    pthread_mutex_init(&s_notify_mutex, NULL);
    pthread_cond_init(&s_notify_cond, NULL);

    sp_session *session;
    sp_session_config config;

    // Libspotify does not guarantee that the structures are freshly initialized
    memset(&config, 0, sizeof(config));

    string cache_location = path + "/cache";
    string settings_location = path = "/settings";

    config.api_version = SPOTIFY_API_VERSION;
    config.cache_location = cache_location.c_str();
    config.settings_location = settings_location.c_str();
    config.application_key = g_appkey;
    config.application_key_size = g_appkey_size;
    config.user_agent = "TopTracksByRegion";
    config.callbacks = &callbacks;
    config.tracefile = NULL;

    sp_error error = sp_session_create(&config, &session);
    log("Libspotify was initiated");

    if (SP_ERROR_OK != error)
        exitl("failed to create session: %s\n", sp_error_message(error));

    // start the libspotify loop
    libspotify_loop(session);
}
