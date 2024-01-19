package com.hepl.cServerConnector;

import java.io.IOException;
import java.util.List;

public class cServerActions {
    private String host;
    private int port;
    private String login;
    private String password;

    public cServerActions(String host, int port) {
        this.host = host;
        this.port = port;
    }

    public cServerActions(String host, int port,String  login ,String  password) {
        this.host = host;
        this.port = port;
        this.login = login;
        this.password = password;
    }

    public List<String> getMoviesList() throws IOException {
        cServerSocket s = new cServerSocket(host, port);
        cServerPacket p = new cServerPacket();
        //No right needed to get the list of Movies
        p.type = 0x01;
        p.status = 0x01;
        p.payloadSize = 0x00;
        s.send(p);
        //Wait to receive the list of movies
        p = s.receive();
        return p.payload;
    }
    public List<String> getShowList(String movie) throws IOException {
        cServerSocket s = new cServerSocket(host, port);
        cServerPacket p = new cServerPacket();
        //No right needed to get the list of Shows
        p.type = 0x02;
        p.status = 0x01;
        p.addPayload(movie);
        s.send(p);
        //Wait to receive the list of shows
        p = s.receive();
        return p.payload;
    }
    public boolean reserveSeats(int seat) throws IOException {
        cServerSocket s = new cServerSocket(host, port);
        cServerPacket p = new cServerPacket();
        //No right needed to reserve seats
        p.type = 0x03;
        p.status = 0x01;
        p.addPayload(Integer.toString(seat));
        s.send(p);
        cServerPacket packet = s.receive();
        packet.status = 0x01;
        if(packet.status == 0x01)
            return true;
        else
            return false;
    }
}
