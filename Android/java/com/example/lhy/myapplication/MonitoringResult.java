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
import android.view.LayoutInflater;
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

public class MonitoringResult extends AppCompatActivity {

    TextView text;
    Intent intent;

    Socket mSock = null;
    BufferedReader mReader;
    BufferedWriter mWriter;
    String Message = "";
    String serverAddr = "164.125.236.69";    //"164.125.236.99";
    String strPort = "8080";
    String mRecvData = "";
    String select;
    int number;
    String index;
    //CheckRecv mCheckRecv = null;
    final String list[] = new String[100];

    Handler mReceiver = new Handler(){
        public void handleMessage(Message msg)
        {
            text.setText("");
            text.setText(Message);
        }
    };

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.monitoring_result);

        text = (TextView) findViewById(R.id.textView);
        text.setMovementMethod(new ScrollingMovementMethod());

        if (mSock != null) return;

        new ConnectTask().execute(serverAddr, strPort);
        new SendTask().execute("2", "start");
        new SendTask().execute("5" , "start");

    }

    public void onClick(View view) {

        number = 0;
        while(list[number] != null)
        {
            number++;
        }

        final String[] items = new String[number];

        for(int i=0;i<number;i++)   items[i] = list[i];

        AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(
                this);

        // 제목셋팅
        alertDialogBuilder.setTitle("선택 목록 대화 상자");
        alertDialogBuilder.setItems(items,
                new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog,
                                        int id) {

                        // 프로그램을 종료한다
                        Toast.makeText(getApplicationContext(),
                                items[id] + " 선택했습니다.",
                                Toast.LENGTH_SHORT).show();
                        new SendTask().execute(index , items[id]);
                        dialog.dismiss();
                    }
                });

        // 다이얼로그 생성
        AlertDialog alertDialog = alertDialogBuilder.create();

        switch (view.getId()) {
            case R.id.btn_filter:
                Log.d("tag" , "after btn_filter");
                index = "4";
                alertDialog.show();
                break;

            case R.id.btn_monitor:

                text.setText("");
                index = "3";
                alertDialog.show();
                break;

            case R.id.btn_add:
                AlertDialog.Builder alert = new AlertDialog.Builder(this);

                alert.setTitle("추가할 도메인 등록");
                alert.setMessage("도메인 이름을 입력하세요");

                // Set an EditText view to get user input
                final EditText input = new EditText(this);
                alert.setView(input);

                alert.setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int whichButton) {
                        String value = input.getText().toString();
                        Log.d("vaule" , value);
                        new SendTask().execute("1" , value.toString());
                        // Do something with value!
                    }
                });


                alert.setNegativeButton("Cancel",
                        new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int whichButton) {
                                // Canceled.
                            }
                        });

                alert.show();
                break;

            case R.id.btn_remove:
                AlertDialog.Builder alert2 = new AlertDialog.Builder(this);

                alert2.setTitle("제거할 도메인");
                alert2.setMessage("제거할 도메인을 입력하세요");

                // Set an EditText view to get user input
                final EditText input2 = new EditText(this);
                alert2.setView(input2);

                alert2.setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int whichButton) {
                        String value = input2.getText().toString();
                        Log.d("vaule" , value);
                        new SendTask().execute("6" , value.toString());
                        // Do something with value!
                    }
                });


                alert2.setNegativeButton("Cancel",
                        new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int whichButton) {
                                // Canceled.
                            }
                        });

                alert2.show();

                break;
            case R.id.btn_restore:
                index = "6";
                alertDialog.show();

                break;
        }
    }

    public class ConnectTask extends AsyncTask<String, String, String> {
        @Override
        protected String doInBackground(String... arg) {
            try {
                Log.d("ConnectTask" , "Connect Start");
                int nPort = Integer.parseInt(arg[1]);

                mSock = new Socket(arg[0], nPort);

                mWriter = new BufferedWriter(new OutputStreamWriter(mSock.getOutputStream()));

            } catch (Exception e) {
                Log.d("tag", "Socket connect error");
                return "Connect fail";
            }
            return "Connect Succeed";
        }
    }

    public class SendTask extends AsyncTask<String, String, String> {
        public String sendMessage;

        @Override
        protected String doInBackground(String... arg) {

            if (mWriter == null)
                return "Can not Send";

            try {
                if(mSock != null)   mReader = new BufferedReader(new InputStreamReader(mSock.getInputStream()));
                else
                {
                    mSock = new Socket(serverAddr , 8080);
                    mReader = new BufferedReader(new InputStreamReader(mSock.getInputStream()));
                }

                PrintWriter out = new PrintWriter(mWriter, true);
                sendMessage = arg[0] + ";" + arg[1];
                out.println(sendMessage);

                Log.d("tag", "after Send Message");

                switch (Integer.parseInt(arg[0])) {
                    case 1:{
                        Log.d("tag" , "arg[1] = " + arg[1]);
                        break;
                    }
                    case 2: {                                                                       // 전체 패킷
                        Log.d("tag", "arg[0] = " + arg[0]);
                        Message = "";
                        while (!(mRecvData = mReader.readLine()).equals("end")) {
                            Log.d("tag", "Recv Data = " + mRecvData);
                            Message = Message + mRecvData + "\n";
                        }
                        Log.d("tag", "after Recv Data");
                        break;
                        //Toast.makeText(MonitoringResult.this , "Message = " + Message , Toast.LENGTH_SHORT);
                    }
                    case 3: {
                        Log.d("tag", "arg[0] = " + arg[0]);// 모니터링 결과
                        Message = "";
                        while (!(mRecvData = mReader.readLine()).equals("end")) {
                            Log.d("tag", "Recv Data = " + mRecvData);
                            Message = Message + mRecvData + "\n";
                        }
                        break;
                    }
                    case 4: {
                        Log.d("tag", "arg[0] = " + arg[0]);//필터링b

                        break;
                    }
                    case 5:                                                                         //ip 리스트 출력
                    {
                        Log.d("tag", "arg[0] = " + arg[0]);
                        int i=0;
                        while (!(mRecvData = mReader.readLine()).equals("end")) {
                            list[i] = mRecvData;
                            Log.d("tag" , "list[" + i + "] =" + list[i]);
                            i++;
                        }
                        Log.d("tag" , "list end");
                        break;
                    }
                    case 6: {
                        Log.d("tag", "arg[0] =" + arg[0]);
                        Log.d("tag", "arg[1] =" + arg[1]);
                        break;
                    }
                }
                mReceiver.sendEmptyMessage(0);

            } catch (Exception e) {
                e.printStackTrace();
                return "send fail";

            }
            return "Send Success";
        }
    }

    public class CloseTask extends AsyncTask<String, String, String> {
        @Override
        protected String doInBackground(String... arg) {
            try {
                if (mSock != null) {
                    mSock.close();
                    mSock = null;
                }
            } catch (Exception e) {
                Log.d("tag", "Socket close error");
                return "Close error";
            }
            return "Close Success";
        }
    }
}