package com.example.lhy.myapplication;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.Toast;

/**
 * Created by LHY on 2016-10-13.
 */

public class CurrentList extends AppCompatActivity{

    RadioButton radio;
    int number;
    String str[];
    RadioGroup ll;
    Intent intent;
    String index;

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.currentlist);
        intent = getIntent();
        ll = new RadioGroup(this);

        str = intent.getExtras().getStringArray("IPlist");
        index = intent.getStringExtra("index");

        number = 0;
        while(str[number] != null)  number++;

        Log.d("index" , "index = " + index);

        for (int row = 0; row <= number/2; row++) {
            ll.setOrientation(LinearLayout.HORIZONTAL);

            for (int i = 1; i <= 2; i++) {
                if(str[row*2 + i-1] != null) {
                    RadioButton rdbtn = new RadioButton(this);
                    rdbtn.setId((row * 2) + i);
                    rdbtn.setText(str[row * 2 + i - 1]);
                    Log.d("button", str[i - 1]);
                    ll.addView(rdbtn);
                }
            }
            ((ViewGroup) findViewById(R.id.radioGroup)).addView(ll);
        }
    }

    public void onClick(View view) {
        RadioButton rb = (RadioButton)findViewById(ll.getCheckedRadioButtonId());
        String imsi = rb.getText().toString();

        Log.d("imsi" , imsi);

        Intent intentFiltering = new Intent(this , FilteringResult.class);

        intentFiltering.putExtra("list" , imsi);
        intentFiltering.putExtra("index" , index);

        startActivity(intentFiltering);

        finish();
    }
}
