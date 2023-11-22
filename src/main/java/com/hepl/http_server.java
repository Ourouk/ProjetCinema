package com.hepl;

import com.hepl.customHttpServer.httpMain;

public class http_server {
    public static void main(String[] args) {
        httpMain http_server = new httpMain();
        http_server.run();
    }
}