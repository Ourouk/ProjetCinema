package com.hepl.customHttpServer;

import com.hepl.socket.crypto.ssl.secureSocketListener;
import com.hepl.socket.socketListener;

public class httpMain implements Runnable{

    @Override
    public void run() {
        System.out.println("Launching Custom Http Server made in Java");
        //It is to note that the listener can launch itself some thread
        int port = 8090;
        int ssl_port = 8091;

        socketListener listener_thread = new socketListener(port,4, socketListener.availableHandler.httpClientHandlerThread);
        //This code launch both socket to receive connection
        Thread http_listening_thread = new Thread(listener_thread);
        http_listening_thread.start();
        System.out.println("The server url is http://127.0.0.1:"+port);

        secureSocketListener SSLlistener_thread = new secureSocketListener(ssl_port,1,false, secureSocketListener.availableHandler.httpClientHandlerThread);
        Thread https_listener_thread = new Thread(SSLlistener_thread);
        https_listener_thread.start();

        System.out.println("The secured server url is https://127.0.0.1:"+ssl_port);
    }
}
