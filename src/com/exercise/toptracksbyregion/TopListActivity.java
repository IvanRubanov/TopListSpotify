package com.exercise.toptracksbyregion;

import android.app.ListActivity;
import android.app.ProgressDialog;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

/**
 * List activity responsible for displaying top tracks list.
 */
public class TopListActivity extends ListActivity {

    public static final String REGION_CODE_EXTRA = "TOP_TRACK_BY_REGION_REGION_CODE_EXTRA";
    
    private TopAdapter mAdapter;
    private String mRegionCode;
    
    private ProgressDialog mProgressDialog;
    // Delegate for on got top list event.
    private GetTopTracksListener mListener;
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.base_list_activity);
        
        mAdapter = new TopAdapter();
        setListAdapter(mAdapter);
        
        mRegionCode = getIntent().getExtras().getString(REGION_CODE_EXTRA);
        
        mListener = new GetTopTracksListener() {
            
            @Override
            public void onGotTop(String[] top) {
                if(mProgressDialog != null) {
                    mProgressDialog.cancel();
                }
                mAdapter.setTracks(top);
            }
        };
        
        mProgressDialog = new ProgressDialog(this);
        mProgressDialog.setMessage(getString(R.string.fetching_top));
        
        mProgressDialog.show();
        
        LibSpotifyWrapper.getTop(mRegionCode, mListener);
    }
    
    /*
     * Adapter.
     */
    private class TopAdapter extends BaseAdapter {

        private class ViewHolder {
            public final TextView mTrackView;

            public ViewHolder(TextView trackView) {
                mTrackView = trackView;
            }
        }
        
        private String[] mTopTracks;
        
        public void setTracks(String[] tracks) {
            mTopTracks = tracks;
            notifyDataSetChanged();
        }
        
        @Override
        public int getCount() {
            return mTopTracks != null ? mTopTracks.length : 0;
        }

        @Override
        public Object getItem(int position) {
            return mTopTracks[position];
        }

        @Override
        public long getItemId(int position) {
            return position;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            TextView track;
            if (convertView == null) {
                LayoutInflater inflater = LayoutInflater.from(parent.getContext());
                convertView = inflater.inflate(android.R.layout.simple_list_item_1, parent, false);
                track = (TextView)convertView.findViewById(android.R.id.text1);
                convertView.setTag(new ViewHolder(track));
            } else {
                ViewHolder viewHolder = (ViewHolder) convertView.getTag();
                track = viewHolder.mTrackView;
            }
            
            String trackName = (String)getItem(position);
            track.setText(trackName);
            return convertView;
        }
        
    }
}
