package com.exercise.toptracksbyregion;

import android.os.Handler;

/**
 * Wrapper for libspotify.so all communication with native code goes through this class.
 */
public class LibSpotifyWrapper {
    /**
     * Handler for execution code in UI thread.
     */
    private static Handler mHandler;
    /**
     * Listener for login events.
     */
    private static LoginListener mLoginListener;
    /**
     * Listener for get top list events.
     */
    private static GetTopTracksListener mGetTopListener;

    /**
     * Initialize and start main loop. 
     * @param loader Loader
     * @param storagePath Staorage path
     */
    native public static void init(ClassLoader loader, String storagePath);

    native private static void login(String username, String password);
    
    native private static void gettop(String region);
    
    /**
     * Setter for handler.
     * @param handler Handler
     */
    public static void setHandler(Handler handler) {
        mHandler = handler;
    }
    
    /**
     * Sign user in using libspotify.
     * @param username String
     * @param password String
     * @param loginListener LoginListener
     */
    public static void loginUser(final String username, final String password, 
            final LoginListener loginListener) {
        new Thread(new Runnable() {
            
            @Override
            public void run() {
                mLoginListener = loginListener;
                login(username, password);
            }
        }).start();
        
    }

    /**
     * Notify listeners about login results. Used by native code.
     */
    public static void onLogin(final boolean success, final String message) {
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                if (success) {
                    if(mLoginListener != null) {
                        mLoginListener.onLogin();
                    }
                } else {
                    if(mLoginListener != null) {
                        mLoginListener.onLoginFailed(message);
                    }
                }
            }
        });

    }
    
    /**
     * Get top track list from libspotify.
     * @param region String 2 chars country code
     * @param listener GetTopTracksListener
     */
    public static void getTop(final String region, final GetTopTracksListener listener) {
        new Thread(new Runnable() {
            
            @Override
            public void run() {
                mGetTopListener = listener;
                gettop(region);
            }
        }).start();
    }
    
    /**
     * Notify listeners about get top track list results. Used by native code.
     */
    public static void onGotTopList(final String[] top) {
        mHandler.post(new Runnable() {
            
            @Override
            public void run() {
                if(mGetTopListener != null) {
                    mGetTopListener.onGotTop(top);
                }
            }
        });
    }
}
