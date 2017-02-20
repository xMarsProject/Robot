package com.example.vincent.controlrobot;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.Button;
import android.util.Log;

import android.view.View;
import android.view.View.OnClickListener;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.TextView;
import android.widget.Toast;
import android.view.MotionEvent;


public class MainActivity extends AppCompatActivity implements View.OnTouchListener {

    Client c=null;
    Thread t=null;
    FrameLayout Grille;
    int x1;
    int y1;
    int ncase1=-1;

    @Override
    public boolean onTouch(View v, MotionEvent event) {

        int x = (int) event.getX();
        int y = (int) event.getY();
        if (y<0 || x<0) return false;
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                x1=x;
                y1=y;
                Log.v("TAG", "x1="+x1);
                Log.v("TAG", "y1="+y1);
                //Log.v("TAG", "touched down");
                break;
            case MotionEvent.ACTION_MOVE:
                int NbcaseX=11;
                int NbcaseY=23;

                int x2=x;
                int y2=y;
                int dx=x2-x1;
                int dy=y2-y1;
                int cx=Grille.getWidth()/NbcaseX;
                int cy=Grille.getHeight()/NbcaseY;
                int caseX=dx/cx;
                int caseY=dy/cy;
                int ncase0=(NbcaseY-1)/2*NbcaseX-(NbcaseX-1)/2;
                int ncase=ncase0+caseX+caseY*NbcaseX;
                if (ncase>0 && ncase<(NbcaseX*NbcaseY) && ncase!=ncase1) {
                    Log.v("tag", "ncase=" + ncase); //1497 Y
                    if (c!=null && c.os!=null) {
                        if (ncase<10)
                            c.envoi("00"+ncase+";");
                        else
                        if (ncase<100)
                            c.envoi("0"+ncase+";");
                        else
                            c.envoi(ncase+";");
                    }
                    ncase1=ncase;
                }
                break;
            case MotionEvent.ACTION_UP:
                if (c!=null && c.os!=null)
                        c.envoi("S;");
                break;
        }

        return true;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);
        Button button1=(Button)findViewById(R.id.connect); // Récupération de l'instance bouton 1
        Grille=(FrameLayout) findViewById(R.id.Grille);
        Grille.setOnTouchListener(this);

        button1.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View view) {
                if (view.getId()==R.id.connect){ // C'est notre bouton ? oui, alors affichage d'un message
                    EditText IP=(EditText)findViewById(R.id.IP);
                    EditText port=(EditText)findViewById(R.id.port);
                    c=new Client();
                    c.setConfig(IP.getText().toString(),port.getText().toString());
                    t = new Thread(c);
                    t.start();
                    int i=0;
                    while(c.getStatut()==0 && i<5) {
                        try {
                            Thread.sleep(1000);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                        i++;
                    }
                    if (c.getStatut()==1)
                        Toast.makeText(MainActivity.this,"Connecté", Toast.LENGTH_SHORT).show();
                    else
                        Toast.makeText(MainActivity.this,"Echec", Toast.LENGTH_SHORT).show();
                }
            }
            }); // Positionnons un listener sur ce bouton

        Log.v("Finish","finish");
    }
}
