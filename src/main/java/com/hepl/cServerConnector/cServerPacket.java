package com.hepl.cServerConnector;

import java.util.List;

public class cServerPacket {
    byte type;
    byte status;
    int payloadSize;
    List<String> payload;
    public cServerPacket() {
        this.payloadSize = 0;
        this.payload = new java.util.ArrayList<String>();
    }
    public void addPayload(String data) {
        payload.add(data);
        payloadSize += data.length();
        if(payload.size() > 1) {
            payloadSize++; // Add the comma
        }
    }

    public byte[] getPayloadAsByteArray() {
        StringBuilder sb = new StringBuilder();
        for (String data : payload) {
            sb.append(data).append(",");
        }
        if (sb.length() > 0) {
            sb.deleteCharAt(sb.length() - 1); // Remove the last comma
        }
        String payloadString = sb.toString();
        System.out.println("Payload: " + payloadString);
        return payloadString.getBytes();
    }
    public void setPayloadFromByteArray(byte[] payload) {
        this.payload = new java.util.ArrayList<String>();
        String payloadString = new String(payload);
        System.out.println("Payload: " + payloadString);
        String[] payloadArray = payloadString.split(",");
        for (String data : payloadArray) {
            this.payload.add(data);
        }

    }
}
