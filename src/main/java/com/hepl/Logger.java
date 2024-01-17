package com.hepl;

public class Logger {
    public static void log(String msg)
    {
        System.out.println("Thread " + Thread.currentThread().getId() + " at " + java.time.LocalDateTime.now().getHour()+":"+java.time.LocalDateTime.now().getMinute()+ ":"+ java.time.LocalDateTime.now().getSecond() + " : " +  msg);
    }
    /**
     * Code generated via ChatGPT to improve the error message handling
     * @param msg
     * @param gravity
     */
    public static void logWithGravity(String msg, Gravity gravity) {
        String gravityColor = "";
        switch (gravity) {
            case LOW:
                gravityColor = "\u001B[32m"; // Green color
                break;
            case MEDIUM:
                gravityColor = "\u001B[33m"; // Yellow color
                break;
            case HIGH:
                gravityColor = "\u001B[31m"; // Red color
                break;
        }

        String resetColor = "\u001B[0m"; // Reset to default color
        String logMessage = gravityColor + "Thread " + Thread.currentThread().getId() + " at " +
                java.time.LocalDateTime.now().getHour() + ":" +
                java.time.LocalDateTime.now().getMinute() + ":" +
                java.time.LocalDateTime.now().getSecond() + " : " + msg + resetColor;

        System.out.println(logMessage);
    }

    public enum Gravity {
        LOW,
        MEDIUM,
        HIGH
    }
}
