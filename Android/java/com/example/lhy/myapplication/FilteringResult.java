package com.example.lhy.myapplication;

import android.app.Dialog;
import android.app.DialogFragment;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import org.w3c.dom.Text;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.URLEncoder;

import static android.R.attr.id;

/**
 * Created by LHY on 2016-10-11.
 */

public class FilteringResult extends AppCompatActivity {

    Intent intent;
    EditText et;

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.filtering_result);
        et = (EditText)findViewById(R.id.editText);
    }

    public void onClick(View view) {
        Intent intent = new Intent(this , MonitoringResult.class);

        switch (view.getId())
        {
            case R.id.btn_add:
                Log.d("input" , et.getText().toString());
                intent.putExtra("edit" , et.getText().toString());
                startActivity(intent);
                finish();
                break;

            case R.id.btn_remove:
                Log.d("input" , "remove");
                intent.putExtra("edit" , "remove");
                startActivity(intent);
                finish();
                break;
        }
    }
}