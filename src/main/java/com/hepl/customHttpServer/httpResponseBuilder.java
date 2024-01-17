/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package com.hepl.customHttpServer;

import java.io.BufferedWriter;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;

/**
 *
 * @author Andrea
 */
public class httpResponseBuilder {
    public httpResponseBuilder()
    {
        
    }
    public httpResponseBuilder(byte[] Content)
    {
        this.CreateDefaultHeader();
        this.setContent(Content);
        this.setSize(this.getContent().length);
    }
    public httpResponseBuilder(byte[] Content,String content_type)
    {
        this.CreateDefaultHeader();
        if(!content_type.isBlank())
            this.setContent_type(content_type);
        this.setContent(Content);
        this.setSize(this.getContent().length);
    }
    private void CreateDefaultHeader()
    {
        this.setVersion("HTTP/1.1");
        this.setCode(200);
        this.setHuman_readable_code("ok");
        this.setContent_type("Content-Type: text/html; charset=utf-8");
    }
    public void send(DataOutputStream o) throws IOException
    {
        //DONE FIX THE MISUNDERSTANDING OF the content length
        BufferedWriter out = new BufferedWriter(new OutputStreamWriter(o));
        out.write(this.getVersion() + " " + this.getCode() + " " + this.getHuman_readable_code() + "\r\n");
        out.write(this.getContent_type() +"\r\n");
        out.write("Content-Length: " + this.getSize() + "\r\n\r\n");
        for (byte b : this.getContent()) {
            out.append((char)b);
        }
        out.flush();
    }
    /**e
     * @return the version
     */
    public String getVersion() {
        return version;
    }

    /**
     * @param version the version to set
     */
    public void setVersion(String version) {
        this.version = version;
    }

    /**
     * @return the code
     */
    public int getCode() {
        return code;
    }

    /**
     * @param code the code to set
     */
    public void setCode(int code) {
        this.code = code;
    }

    /**
     * @return the human_readable_code
     */
    public String getHuman_readable_code() {
        return human_readable_code;
    }

    /**
     * @param human_readable_code the human_readable_code to set
     */
    public void setHuman_readable_code(String human_readable_code) {
        this.human_readable_code = human_readable_code;
    }

    /**
     * @return the content_type
     */
    public String getContent_type() {
        return  content_type;
    }

    /**
     * @param content_type the content_type to set
     */
    public void setContent_type(String content_type) {
        this.content_type = content_type;
    }

    /**
     * @return the size
     */
    public int getSize() {
        return size;
    }

    /**
     * @param size the size to set
     */
    //DONE https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Content-Length contain lenght in byte
    public void setSize(int size) {
        this.size = size;
    }

    /**
     * @return the content
     */
    public byte[] getContent() {
        return content;
    }

    /**
     * @param content the content to set note that it set the size
     */
    public void setContent(byte[] content) {
        this.content = content;
        this.setSize(content.length);
    }
    private String version;
    private int code;
    private String human_readable_code;
    private String content_type;
    private int size;
    private byte[] content;
}
