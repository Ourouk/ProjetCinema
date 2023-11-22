/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package com.hepl.socket;
import com.hepl.customHttpServer.httpClientHandlerThread;
import com.hepl.socket.crypto.ssl.secureSocketListener;

import java.io.IOException;
import java.net.*;
import java.util.concurrent.*;
import java.util.logging.FileHandler;
import java.util.logging.Level;
import java.util.logging.Logger;


/**
 *
 * @author Andrea
 */

public class socketListener implements Runnable{
    private ServerSocket serverSocket;
    private int port;
    private int pool_size;
    public enum availableHandler {
        httpClientHandlerThread,ftpClientHandlerThread
    }
    availableHandler targetHandler = availableHandler.httpClientHandlerThread;
    public socketListener(int port, int poolsize)
    {
        //Init the debug logger
        this.port = port;
        this.pool_size = poolsize;

        Logger logger = Logger.getLogger(secureSocketListener.class.getName());
        try {
            logger.addHandler(new FileHandler("logs/" + "socket_listener" + ".log"));
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
    public socketListener(int port, int poolsize,availableHandler targetHandler)
    {
        //Init the debug logger
        this.port = port;
        this.pool_size = poolsize;
        this.targetHandler = targetHandler;

        Logger logger = Logger.getLogger(secureSocketListener.class.getName());
        try {
            logger.addHandler(new FileHandler("logs/" + "socket_listener" + ".log"));
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
    @Override
    public void run() {
       ExecutorService executor_reader = Executors.newFixedThreadPool(pool_size);
        try {
            serverSocket = new ServerSocket(port);
        } catch (IOException ex) {
            throw new RuntimeException(ex);
        }
       while(true)
       {
           //Launch New thread for each connection
           try {
               if(targetHandler == availableHandler.httpClientHandlerThread) {
                   httpClientHandlerThread reading_thread = new httpClientHandlerThread(serverSocket.accept());
                   executor_reader.execute(reading_thread);
               }
//             if(targetHandler == availableHandler.ftpClientHandlerThread) {
//                 ftpClientControlHandlerThread reading_thread = new ftpClientControlHandlerThread(serverSocket.accept());
//                 executor_reader.execute(reading_thread);
//               }
           } catch (IOException ex) {
               Logger.getLogger(socketListener.class.getName()).log(Level.SEVERE, null, ex);
           }
       }
    }
}
