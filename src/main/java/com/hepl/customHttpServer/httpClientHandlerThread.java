    /*
     * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
     * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
     */
    package com.hepl.customHttpServer;
    import com.hepl.Logger;
    import com.hepl.socket.crypto.ssl.secureSocketListener;

import java.net.*;
    import java.io.*;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
    import java.nio.file.Path;
    import java.nio.file.Paths;
    import java.util.HashMap;

    import java.util.Map;
    import java.util.logging.FileHandler;
    import java.util.logging.Level;

    /**
     *
     * @author Andrea
     */
    public class httpClientHandlerThread implements Runnable{
        private Socket socket = null;
        private BufferedReader input = null;
        private DataOutputStream output = null;
        private InputStream input_stream = null;



        //Default files
        static final File DIR_ROOT = new File("www");
        static final File DEFAULT_FILE = new File(DIR_ROOT.getAbsolutePath(),"index.html");
        static final File FILE_NOT_FOUND = new File(DIR_ROOT.getAbsolutePath(),"404.html");
        static final File METHOD_NOT_SUPPORTED = new File(DIR_ROOT.getAbsolutePath(),"not_supported.html");

        //Http request parsing variables
        private String httpMethod;
        private String httpPath;
        private String httpVersion;
        private String httpParts[];
        private String requestLine[];
        public String requestBody;





        private Map<String,String> httpHeaders = new HashMap<String,String>();



        public httpClientHandlerThread(Socket s)
        {
            java.util.logging.Logger logger = java.util.logging.Logger.getLogger(secureSocketListener.class.getName());
            try {
                logger.addHandler(new FileHandler("logs/" + "https_server_thread" + ".log"));
            } catch (IOException e) {
                throw new RuntimeException(e);
            }

            if(DIR_ROOT.mkdir())
            {
                Logger.log("Created Directory Containing HTML files");
            }
            this.socket = s;
        }
        @Override
        public void run() {
            Logger.log("Http Thread Initialised : " + socket.getInetAddress().getHostAddress() + ':' + socket.getPort());
            try {
                this.input_stream = socket.getInputStream();
                input = new BufferedReader(new InputStreamReader(input_stream,StandardCharsets.UTF_8));
                output = new DataOutputStream(socket.getOutputStream());

                //Read the whole content
                StringBuilder sb = new StringBuilder();
                String inputLine;
                while ((inputLine = input.readLine()) != null) {
                    sb.append(inputLine);
                    sb.append("\r\n");
                    if (inputLine.isEmpty()) {
                            break;
                    }
                }
                String httpCommand = sb.toString();

                // Parse the HTTP command
                httpParts = httpCommand.split("\r\n");
                requestLine = httpParts[0].split("\\s+");
                httpMethod = requestLine[0];
                httpPath = requestLine[1];
                httpVersion = requestLine[2];
                httpHeaders = new HashMap<>();
                for (int i = 1; i < httpParts.length - 1; i++) {
                    String[] header = httpParts[i].split(":\\s+");
                    httpHeaders.put(header[0], header[1]);
                }
                if(httpMethod.equals("POST"))
                {
                    try {
                        int numChars = Integer.parseInt(httpHeaders.get("Content-Length"));
                        char[] buff = new char[numChars];
                        int numRead = 0;
                        while (numRead < numChars) {
                            int count = input.read(buff, numRead, numChars - numRead);
                            numRead += count;
                        }
                        requestBody = new String(buff);
                    }catch (IOException e) {
                        System.out.println("Error while reading POST body");
                        throw new RuntimeException(e);
                    }
                }else {
                    requestBody = "";
                }

                Logger.log("Receive Command : " + httpCommand);
                Logger.log("Receive POST Body : " + requestBody);
                switch (httpMethod) {
                    case "GET" -> this.GEThandler();
//                    case "HEAD" -> this.HEADhandler();
                    case "POST" -> this.POSThandler();
                    default -> this.NOTSUPPORTEDdhandler();
                }
                output.flush();
                socket.close();
            }catch (Exception ex) {
                ex.printStackTrace();
                java.util.logging.Logger.getLogger(httpClientHandlerThread.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
        public void GEThandler() throws FileNotFoundException, IOException, SocketException
        {
            Logger.log("Receive GET Command by " + socket.getInetAddress().getHostAddress() + " on \"" +  httpPath + "\"");
            httpSmartHttpHandler smartHttp;
            smartHttp = new httpSmartHttpHandler(this,pathHandler());
            httpResponseBuilder response = new httpResponseBuilder(smartHttp.getFileContent());
            response.send(output);
        }
//        public void HEADhandler() throws IOException
//        {
//            httpSmartHttpHandler smartHttp = new httpSmartHttpHandler(pathHandler().toPath());
//            httpResponseBuilder response = new httpResponseBuilder(smartHttp.getFileContent());
//            response.send(output);
//        }
        public void POSThandler() throws IOException
        {
            Logger.log("Receive POST Command by " + socket.getInetAddress().getHostAddress() + " on \"" +  httpPath + "\"");
            httpSmartHttpHandler smartHttp = new httpSmartHttpHandler(this,pathHandler());
            httpResponseBuilder response = new httpResponseBuilder(smartHttp.getFileContent());
            response.send(output);
        }
        public void NOTSUPPORTEDdhandler() throws IOException
        {
            httpResponseBuilder response = new httpResponseBuilder(Files.readAllBytes(Paths.get(METHOD_NOT_SUPPORTED.getAbsolutePath())));
            try {
                response.send(output);
            }catch (Exception ex)
            {
                java.util.logging.Logger.getLogger(httpClientHandlerThread.class.getName()).log(Level.SEVERE, null, ex);
            }
        }

        /**
         *
         * @return
         * @throws IOException
         */
        private Path pathHandler() throws IOException
        {
                Path file_path;
                if(!this.httpPath.equals("/"))
                    file_path = Paths.get(DIR_ROOT.getAbsolutePath(),httpPath);
                else
                     file_path = DEFAULT_FILE.toPath();
                return file_path;
        }
    }