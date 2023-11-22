package com.hepl.socket.crypto.ssl;
import com.hepl.customHttpServer.httpClientHandlerThread;
import com.hepl.socket.socketListener;

import java.io.*;
import java.security.*;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.logging.FileHandler;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.net.ssl.*;

//Note this class is there to create a listening socket in ssl, I generated
// keytool -genkeypair -alias server -keyalg RSA -keysize 2048 -validity 365 -keystore server_keystore.jks
// keytool -exportcert -alias server -keystore server_keystore.jks -file server_certificate.cer


public class secureSocketListener implements Runnable{
    int port;
    int pool_size;
    boolean need_auth = false;
    public enum availableHandler {
        httpClientHandlerThread,ftpClientHandlerThread
    }
    availableHandler targetHandler = availableHandler.httpClientHandlerThread;
    public secureSocketListener(int port, int poolsize )
    {
        this.port = port;
        this.pool_size = poolsize;
    }
    public secureSocketListener(int port, int poolsize, boolean need_auth,availableHandler availablehandler)
    {
        this.port = port;
        this.pool_size = poolsize;
        this.need_auth = need_auth;
        this.targetHandler = availablehandler;
    }
    @Override
    public void run() {

        Logger logger = Logger.getLogger(secureSocketListener.class.getName());
        logger.setLevel(Level.INFO);
        try {
            logger.addHandler(new FileHandler("logs/" + "https_listener_thread" + ".log"));
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

        try {
            char[] password = "sYCqG5xZzpMmjrQDAWspVH2jcoQu6EDJEwgZuYEw6BEKpMyoshfQPlc1vu3J".toCharArray();
            KeyStore keyStore = KeyStore.getInstance("JKS");
            FileInputStream fis = new FileInputStream("ssl/server_keystore.jks");
            keyStore.load(fis, password);

            // Create the SSL context using the loaded keystore
            KeyManagerFactory keyManagerFactory = KeyManagerFactory.getInstance(KeyManagerFactory.getDefaultAlgorithm());
            keyManagerFactory.init(keyStore, password);
            SSLContext sslContext = SSLContext.getInstance("TLS");
            sslContext.init(keyManagerFactory.getKeyManagers(), null, new SecureRandom());

            // Create the SSL server socket and bind it to the specified port
            SSLServerSocket serverSocket;
            serverSocket = (SSLServerSocket) sslContext.getServerSocketFactory().createServerSocket(port);
            //Put this line if you want only authenticated client to be able to be connected
            if(this.need_auth)
                serverSocket.setNeedClientAuth(true);

    // Listening
            ExecutorService executor_reader = Executors.newFixedThreadPool(4);

            while (true) {
                if(targetHandler == availableHandler.httpClientHandlerThread) {
                    httpClientHandlerThread reading_thread = new httpClientHandlerThread(serverSocket.accept());
                    executor_reader.execute(reading_thread);
                }
                //if(targetHandler == availableHandler.ftpClientHandlerThread) {
                //    ftpClientControlHandlerThread reading_thread = new ftpClientControlHandlerThread(serverSocket.accept(),sslContext);
                //    executor_reader.execute(reading_thread);
                //}
            }
        } catch (Exception e) {
            Logger.getLogger(socketListener.class.getName()).log(Level.SEVERE, null, e);
        }
    }
}
