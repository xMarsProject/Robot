package com.example.vincent.controlrobot;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.ByteArrayOutputStream;
import java.io.InputStream;

import android.util.Log;

public class Client implements Runnable{
    private int statut;
    private String ip;
    private int port;
    DataOutputStream os = null;
    private DataInputStream is = null;
    private Socket socket=null;
    private InetSocketAddress socketAdress=null;


    public Client(){
        statut=0;
    }

    public void setConfig(String Ip, String port)
    {
        this.ip = Ip;
        this.port=Integer.parseInt(port);
    }

    public int getStatut()
    {
        return statut;
    }

    public int connect()
    {
        int timeout = 10000;
        statut=0;
        try {
            socket = new Socket();
            socket.bind(null);
            Log.v("BotC",ip+":"+port);
            socketAdress = new InetSocketAddress(ip,port);
            socket.connect(socketAdress, timeout);
            os = new DataOutputStream(socket.getOutputStream());
            statut=1;
        } catch (IOException e) {
            Log.e("PAO","IO Exception.", e);
        }
        return statut;
    }

    public void envoi(String s)
    {
        try {
            os.writeBytes(s);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    @Override
    public void run() {
        Log.v("BotC","Démarrage du client réseau");
        connect();
        while (statut<2) try {
            Thread.sleep(1000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

}