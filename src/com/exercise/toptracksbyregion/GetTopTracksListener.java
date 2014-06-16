package com.exercise.toptracksbyregion;

/**
 * Interface for delegate responsible for handling received list of top tracks.
 */
public interface GetTopTracksListener {
    /**
     * Handles top list.
     * @param top String array.
     */
    public void onGotTop(String[] top);
}
