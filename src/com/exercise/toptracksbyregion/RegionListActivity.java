package com.exercise.toptracksbyregion;

import java.util.HashMap;
import java.util.Map;

import android.app.ListActivity;
import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.BaseAdapter;
import android.widget.TextView;

/**
 * List activity responsible for displaying country list.
 */
public class RegionListActivity extends ListActivity {
    private RegionAdapter mAdapter;
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.base_list_activity);
        mAdapter = new RegionAdapter();
        setListAdapter(mAdapter);
        // On region tap handling.
        getListView().setOnItemClickListener(new OnItemClickListener() {

            @Override
            public void onItemClick(AdapterView<?> parent, View view,
                int position, long id) {
                String regionCode = mAdapter.getRegionCode(position);
                Intent intent = new Intent(RegionListActivity.this, TopListActivity.class);
                intent.putExtra(TopListActivity.REGION_CODE_EXTRA, regionCode);
                startActivity(intent);
            }
        });
    }
    
    /*
     * Adapter for region list.
     */
    private class RegionAdapter extends BaseAdapter {

        private class ViewHolder {
            public final TextView mRegionView;

            public ViewHolder(TextView regionView) {
                mRegionView = regionView;
            }
        }
        
        // List of regions
        private Map<String, String> sRegions;
        private String[] sKeys;
        
        public RegionAdapter() {
            // Create list of regions
            sRegions = new HashMap<String, String>(31);
            sRegions.put("AD", getString(R.string.country_ad));
            sRegions.put("DK", getString(R.string.country_dk));
            sRegions.put("FI", getString(R.string.country_fi));
            sRegions.put("IE", getString(R.string.country_ie));
            sRegions.put("LT", getString(R.string.country_lt));
            sRegions.put("MC", getString(R.string.country_mc));
            sRegions.put("NO", getString(R.string.country_no));
            sRegions.put("SG", getString(R.string.country_sg));
            sRegions.put("GB", getString(R.string.country_gb));
            sRegions.put("AU", getString(R.string.country_au));
            sRegions.put("DE", getString(R.string.country_de));
            sRegions.put("FR", getString(R.string.country_fr));
            sRegions.put("IT", getString(R.string.country_it));
            sRegions.put("LU", getString(R.string.country_lu));
            sRegions.put("MX", getString(R.string.country_mx));
            sRegions.put("PL", getString(R.string.country_pl));
            sRegions.put("CH", getString(R.string.country_ch));
            sRegions.put("AT", getString(R.string.country_at));
            sRegions.put("BE", getString(R.string.country_be));
            sRegions.put("ES", getString(R.string.country_es));
            sRegions.put("HK", getString(R.string.country_hk));
            sRegions.put("LV", getString(R.string.country_lv));
            sRegions.put("NL", getString(R.string.country_nl));
            sRegions.put("PT", getString(R.string.country_pt));
            sRegions.put("SE", getString(R.string.country_se));
            sRegions.put("EE", getString(R.string.country_ee));
            sRegions.put("IS", getString(R.string.country_is));
            sRegions.put("LI", getString(R.string.country_li));
            sRegions.put("MY", getString(R.string.country_my));
            sRegions.put("NZ", getString(R.string.country_nz));
            sRegions.put("US", getString(R.string.country_us));
            
            sKeys = sRegions.keySet().toArray(new String[sRegions.size()]);
        }
        
        public String getRegionCode(int position) {
            return sKeys[position];
        }
        
        @Override
        public int getCount() {
            return sRegions.size();
        }

        @Override
        public Object getItem(int position) {
            return sRegions.get(sKeys[position]);
        }

        @Override
        public long getItemId(int arg0) {
            return arg0;
        }

        @Override
        public View getView(int position, View view, ViewGroup parent) {
            TextView region;
            if (view == null) {
                LayoutInflater inflater = LayoutInflater.from(parent.getContext());
                view = inflater.inflate(android.R.layout.simple_list_item_1, parent, false);
                region = (TextView)view.findViewById(android.R.id.text1);
                view.setTag(new ViewHolder(region));
            } else {
                ViewHolder viewHolder = (ViewHolder) view.getTag();
                region = viewHolder.mRegionView;
            }
            
            String regionName = (String)getItem(position);
            region.setText(regionName);
            return view;
        }
        
    }
}
