package com.exercise.toptracksbyregion;

import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;
import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;

/**
 * Login activity.
 */
public class LoginActivity extends Activity {
    
    private EditText mUserName;
    private EditText mPassword;
    private Button mLogin;
    // Listener for on login events.
    private LoginListener mLoginListener;
    
    private ProgressDialog mProgressDialog;
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // If log in already had been done just navigate to region select activity.
        SharedPreferences prefs = getSharedPreferences("topbyresion", Context.MODE_PRIVATE);
        boolean loggedin = prefs.getBoolean("loggedin", false);
        if(loggedin) {
            Intent intent = new Intent(LoginActivity.this, RegionListActivity.class);
            startActivity(intent);
            finish();
        }
        
        setContentView(R.layout.login_activity);
        
        mUserName = (EditText)findViewById(R.id.user_name);
        mPassword = (EditText)findViewById(R.id.password);
        mLogin = (Button)findViewById(R.id.login);
        mLogin.setOnClickListener(new OnClickListener() {
            
            @Override
            public void onClick(View v) {
                attemptLogin();
            }
        });
        
        mLoginListener = new LoginListener() {
            
            @Override
            public void onLoginFailed(String message) {
                if(mProgressDialog != null) {
                    mProgressDialog.cancel();
                }
                Toast.makeText(LoginActivity.this, getString(R.string.login_failed), 
                        Toast.LENGTH_LONG).show();
            }
            
            @Override
            public void onLogin() {
                SharedPreferences prefs = getSharedPreferences("topbyresion", Context.MODE_PRIVATE);
                prefs.edit().putBoolean("loggedin", true).commit();
                
                if(mProgressDialog != null) {
                    mProgressDialog.cancel();
                }
                Intent intent = new Intent(LoginActivity.this, RegionListActivity.class);
                startActivity(intent);
                finish();
            }
        };
    }
    
    private void attemptLogin() {
        mUserName.setError(null);
        mPassword.setError(null);
        
        boolean cancel = false;
        
        String userName = mUserName.getText().toString().trim();
        String password = mPassword.getText().toString().trim();
        
        if(TextUtils.isEmpty(userName)) {
            mUserName.setError(getString(R.string.user_name_filed_empty));
            cancel = true;
        } else if(userName.length() < 3) {
            mUserName.setError(getString(R.string.user_name_filed_lenght));
            cancel = true;
        }
        
        if(TextUtils.isEmpty(password)) {
            mPassword.setError(getString(R.string.password_filed_empty));
            cancel = true;
        } else if(password.length() < 3) {
            mPassword.setError(getString(R.string.password_filed_lenght));
            cancel = true;
        }
        
        if(!cancel) {
            mProgressDialog = new ProgressDialog(this);
            mProgressDialog.setMessage(getString(R.string.login_progress_message));
            
            mProgressDialog.show();
            LibSpotifyWrapper.loginUser(userName, password, mLoginListener);
        }
    }
}
