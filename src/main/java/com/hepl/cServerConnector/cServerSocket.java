package com.hepl.cServerConnector;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.nio.ByteBuffer;

public class cServerSocket {
    private Socket socket;
    public cServerSocket(String host, int port) throws IOException {
        socket = new Socket(host, port);
    }

    public void send(cServerPacket packet) throws IOException {
        OutputStream out = socket.getOutputStream();
        System.out.println("Begin Network Log for send_packet");
        // The header is sent first
        out.write(packet.type);
        out.write(packet.status);
        ByteBuffer bb = ByteBuffer.allocate(8);
        bb.putLong(packet.payloadSize);
        out.write(bb.array());
        System.out.println("Header: Type :" + packet.type + ", Status : " + packet.status + ", Payload Size : " + packet.payloadSize);
        // Then the payload
        if(packet.payloadSize > 0)
            out.write(packet.getPayloadAsByteArray());

        // Logging
        System.out.println("End Network Log for send_packet");
    }

    public cServerPacket receive() throws IOException {
        InputStream in = socket.getInputStream();
        System.out.println("Begin Network Log for receive_packet");
        // The header is received first
        cServerPacket packet = new cServerPacket();
        packet.type = (byte) in.read();
        packet.status = (byte) in.read();
        packet.payloadSize = in.read();
        System.out.println("Header: Type :" + packet.type + ", Status : " + packet.status + ", Payload Size : " + packet.payloadSize);
        // Then the payload
        byte[] payload = new byte[(int) packet.payloadSize];
        int bytesRead = 0;
        while (bytesRead < packet.payloadSize) {
            bytesRead += in.read(payload, bytesRead, (int) (packet.payloadSize - bytesRead));
        }
        packet.setPayloadFromByteArray(payload);
        System.out.println("End Network Log for receive_packet");
        return packet;
    }

    public void close() throws IOException {
        socket.close();
    }
}