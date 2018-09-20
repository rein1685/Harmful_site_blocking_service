package com.example.lhy.myapplication;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {

    TextView text;
    EditText editId;
    EditText editpasswd;
    Intent intent;

    String correctId = "rein1685";
    String correctPasswd = "1516453";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        text = (TextView)findViewById(R.id.textView);
    }

    public void onClick(View view) {
        switch(view.getId()){
            case R.id.btn_login:
                editId = (EditText) findViewById(R.id.edit_id);
                editpasswd = (EditText) findViewById(R.id.edit_passwd);

                if (correctId.equals(editId.getText().toString()) && correctPasswd.equals(editpasswd.getText().toString())) {
                    intent = new Intent(this, MonitoringResult.class);
                    startActivity(intent);
                    finish();
                } else {
                    Toast.makeText(getApplication(), "incorrect ID/PASSWD" ,  Toast.LENGTH_SHORT).show();
                }
                break;

        }
    }
}
