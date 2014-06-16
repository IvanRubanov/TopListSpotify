package com.exercise.toptracksbyregion;

/**
 * Interface for delegate responsible for handling on log in events.
 */
public interface LoginListener {
    /**
     * Login successful.
     */
    void onLogin();
    /**
     * Login failed.
     * @param message String explanation why login failed.
     */
    void onLoginFailed(String message);
}
