/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package com.hepl.customHttpServer;

import com.hepl.Logger;
import com.hepl.cServerConnector.cServerActions;
import com.hepl.customSmtpClient.smtpSender;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.HashMap;
import java.util.List;

/**
 *
 * @author Andrea
 */
public class httpSmartHttpHandler {
    private httpClientHandlerThread httpClientHandlerThread;
    private Path file;
    private FileReader fileReader;
    private Boolean isSmart;

    /**
     *
     * @param path to the file to be sent
     */
    public httpSmartHttpHandler(httpClientHandlerThread httpClient,Path path)
    {
        this.httpClientHandlerThread = httpClient;
        this.setFile(path);
    }

    /**
     * Note : This Method contains the smart file detector
     * @param file_path The path to the file to set
     */
    private void setFile(Path file_path) {
        Path path;
        if(file_path.toFile().exists())
            path = file_path;
        else
            path = com.hepl.customHttpServer.httpClientHandlerThread.FILE_NOT_FOUND.toPath();

        String[] split = path.getFileName().toString().split("[.]");
            setIsSmart(split[1]);
        this.file = file_path;
        try {
            this.fileReader = new FileReader(this.file.toFile());
        } catch (FileNotFoundException e) {
            throw new RuntimeException(e);
        }
    }
    /**
     * @return the file path
     */
    public Path getFile() {
        return file;
    }


    /**
     * @param FileType the FileType to set
     * Here we can parse differently certains types of files depending on their type
     */
    private void setIsSmart(String FileType) {
        switch(FileType){
            case "shtml": //smarthttp
                this.isSmart = true;
                break;
            default:
                this.isSmart = false;
        }
    }

    public byte[] getFileContent() throws IOException
    {
        if(this.isSmart)
            return smartInjectionHandler(Files.readAllBytes(file));
        else {
            return Files.readAllBytes(file);
        }
    }


    /**
     *
     * @param filecontent //Contain the all the content from the shtml file
     * @return modified filecontent
     * @throws IOException
     */
    private byte[] smartInjectionHandler(byte[] filecontent) throws IOException {
        byte[] new_filecontent;
        String buff = new String(filecontent);
        StringBuilder builder = new StringBuilder();
        for(String s : listOfAvailablesFunctions)
        {
            if(buff.contains(s))
            {
                String htmltoAdd = functionCaller(s);
                String[] split = buff.split(s);
                    builder.append(split[0]);
                    builder.append(htmltoAdd);
                    builder.append(split[1]);
                buff = builder.toString();
            }
        }
        new_filecontent = buff.getBytes();
        return new_filecontent;
    }

    // All Smart Handler should be added in the two next method and create a new function to add the content.
    private String[] listOfAvailablesFunctions = new String[]{"<!-- displayMovieList -->","<!-- displayShowList -->","<!-- Add Here the mail sent -->","Error you have called an unset function"};

    private String functionCaller(String s) throws IOException {
        switch(s)
        {
            case "<!-- displayMovieList -->":
                return displayMovieList();
            case "<!-- displayShowList -->":
                return displayShowList();
            case "<!-- Add Here the mail sent -->":
                return mailsent();
            default:
                System.out.println("Error you have called an unset function");
                return null;
        }
    }


    private String displayMovieList(){
    cServerActions cServerActions = new cServerActions("localhost", 5050);
    List<String> listOffMovies;
    try {
            listOffMovies = cServerActions.getMoviesList();
            StringBuilder builder = new StringBuilder();
            builder.append("<form action=\"/show_selector.shtml\" method=\"post\">\n");
            for(String s : listOffMovies) {
                builder.append("<label for=\"").append(s).append("\">\n");
                builder.append("<input type=\"checkbox\" id=\"").append(s).append("\" name=\"options[]\" value=\"").append(s).append("\">\n");
                builder.append(s).append("\n");
                builder.append("</label>\n");
            }
            builder.append("<input type=\"submit\" value=\"Submit\">\n");
            builder.append("</form>\n");
            String result = builder.toString();
            return result;
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        return "Error while getting the list of movies";
    }

    private String displayShowList() {
        cServerActions cServerActions = new cServerActions("localhost", 5050);
        try {
            List<String> listOffShows = cServerActions.getShowList(httpClientHandlerThread.requestBody);
            StringBuilder builder = new StringBuilder();
            builder.append("<form action=\"/thank_you.html\" method=\"post\">\n");
            for(String s : listOffShows) {
                builder.append("<label for=\"").append(s).append("\">\n");
                builder.append("<input type=\"checkbox\" id=\"").append(s).append("\" name=\"options[]\" value=\"").append(s).append("\">\n");
                builder.append(s).append("\n");
                builder.append("</label>\n");
            }
            builder.append("<input type=\"submit\" value=\"Submit\">\n");
            builder.append("</form>\n");
            String result = builder.toString();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        return "Error while getting the list of shows";
    }

    private String mailsent() {
        return "to_implement";
    }

    private HashMap body_parser(String body)
    {
        HashMap form_hashmap = new HashMap();
        String[] body_list = httpClientHandlerThread.requestBody.split("\n");
        for (String line: body_list) {
            String[] split_key = line.split("=");
            form_hashmap.put(split_key[0], split_key[1]);
        }
        return form_hashmap;
    }
}
