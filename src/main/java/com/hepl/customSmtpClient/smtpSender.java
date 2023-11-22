package com.hepl.customSmtpClient;

import com.dumbster.smtp.SimpleSmtpServer;
import com.dumbster.smtp.SmtpMessage;
import com.sun.mail.smtp.SMTPMessage;

import javax.activation.DataHandler;
import javax.activation.FileDataSource;
import javax.mail.*;
import javax.mail.internet.*;
import java.nio.file.Path;
import java.util.Properties;

public class smtpSender implements Runnable{
    private String mailto;
    private String content;
    private Path filepath;
    public smtpSender(String mailto,String content)
    {
        this.mailto = mailto;
        this.content = content;
    }
    public smtpSender(String mailto,String content,Path filepath)
    {
        this.mailto = mailto;
        this.content = content;
        this.filepath = filepath;
    }
    @Override
    public void run() {
        Properties props = new Properties();
        props.put("mail.smtp.host", "smtp.mailtrap.io");
        props.put("mail.smtp.port", "2525");
        props.put("mail.smtp.auth", "true");
        props.put("mail.smtp.starttls.enable", "true");
        props.put("mail.smtp.ssl.protocols", "TLSv1.2");

        Session session = Session.getInstance(props, new Authenticator() {
            protected PasswordAuthentication getPasswordAuthentication() {
                return new PasswordAuthentication("e93f699377b93a", "e312aa829798a3");
            }
        });
        MimeMessage message = new MimeMessage(session);
        try {
            message.setFrom(new InternetAddress("SENDER_EMAIL_ADDRESS"));
            message.addRecipient(Message.RecipientType.TO, new InternetAddress(mailto));
            message.setSubject("Test Email");
            message.setText(content);
            if(filepath != null) {
                message.setDataHandler(new DataHandler(new FileDataSource(filepath.toString())));
                message.setFileName(filepath.toString());
            }
            Transport.send(message);
        } catch (MessagingException e) {
            throw new RuntimeException(e);
        }
    }
}