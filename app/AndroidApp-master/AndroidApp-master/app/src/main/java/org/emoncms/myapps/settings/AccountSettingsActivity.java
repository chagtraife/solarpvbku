package org.emoncms.myapps.settings;

import android.app.Fragment;
import android.app.FragmentTransaction;
import android.content.Context;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AlertDialog;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;

import org.emoncms.myapps.BaseActivity;
import org.emoncms.myapps.EmonApplication;
import org.emoncms.myapps.R;


/**
 * Activity to handle settings
 */
public class AccountSettingsActivity extends BaseActivity {

    private String account;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        account = getIntent().getExtras().getString("account");
        setContentView(R.layout.activity_account_settings);
        setUpToolbar();
        setUpFragments();

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.account_settings, menu);
        return true;
    }



    private void setUpToolbar() {
        Toolbar  toolbar = (Toolbar) findViewById(R.id.main_toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setTitle(R.string.account_settings);
            actionBar.setDisplayHomeAsUpEnabled(true);
        }
    }

    private void setUpFragments() {

        String accountsSettingsTag = "account_settings_fragment";
        Fragment fragAccounts = new AccountSettingsFragment();


        //pass this on as contains accountId
        fragAccounts.setArguments(getIntent().getExtras());


        FragmentTransaction transaction = getFragmentManager().beginTransaction();
        transaction.replace(R.id.prefs, fragAccounts, accountsSettingsTag);

        transaction.commit();

    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.action_delete_account:
                deleteAccount();
                return true;
            case android.R.id.home:
                onBackPressed();
                return true;
        }
        return( super.onOptionsItemSelected(item));
    }

    private void deleteAccount() {

        AlertDialog.Builder builder =
                new AlertDialog.Builder(this);

        builder.setTitle(R.string.settings_title_delete_account);
        builder.setMessage(R.string.settings_confirm_delete_account);
        builder.setPositiveButton(R.string.ok, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialogInterface, int i) {
                SharedPreferences settings = getSharedPreferences("emoncms_account_" + account, Context.MODE_PRIVATE);
                settings.edit().clear().commit();
                //update main setting
                EmonApplication.get().removeAccount(account);
                // go back to the settings page
                onBackPressed();
            }
        });
        builder.setNegativeButton(R.string.cancel, null);
        builder.show();
    }

}