package com.exercise.toptracksbyregion;

import android.app.Application;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.Environment;
import android.os.Handler;
import android.os.Looper;

/**
 * Application object. Used for initialization native libs.
 * Initialize native libs only once. If we initialize libs in Activy or Service
 * and then recreate them application crashes in sp_create_session() call.
 */
public class TopListByRegionApplication extends Application {

    static {
        System.loadLibrary("spotify");
        System.loadLibrary("spotifywrapper");
    }
    
    @Override
    public void onCreate() {
        super.onCreate();
        if (!Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED))
            throw new RuntimeException("Storage card not available");
        new Thread(new Runnable() {
            
            @Override
            public void run() {
                LibSpotifyWrapper.init(LibSpotifyWrapper.class.getClassLoader(), 
                        Environment.getExternalStorageDirectory().getAbsolutePath() + 
                            "/Android/data/com.exercise.toptracksbyregion");
            }
        }).start();
        LibSpotifyWrapper.setHandler(new Handler(Looper.getMainLooper()));
        // Login hasn't been done.
        SharedPreferences prefs = getSharedPreferences("topbyresion", Context.MODE_PRIVATE);
        prefs.edit().putBoolean("loggedin", false).commit();
    }
}
