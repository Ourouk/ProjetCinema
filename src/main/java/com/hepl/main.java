/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Project/Maven2/JavaApp/src/main/java/${packagePath}/${mainClassName}.java to edit this template
 */

package com.hepl;

//import com.hepl.customFtpServer.ftpMain;
import com.hepl.customHttpServer.httpMain;
    /**
 *
 * @author Andrea
 */
public class main {

    public static void main(String[] args) {
//        /**
//         * Testing Function Must be removed from actual final product
//         */
//        testingMain.authServerConnectionTesting() //Testing still needed
//        testingMain.smtpClientTesting(); // Done

        //ftpMain ftp_server = new ftpMain();
        httpMain http_server = new httpMain();
        http_server.run();
        //ftp_server.run();
    }
}
