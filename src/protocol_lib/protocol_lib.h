#ifndef PROTOCOL_LIB_H
#define PROTOCOL_LIB_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX_PAYLOAD_SIZE 2048
struct Parameter {
    char* data; // This will hold the pointer
    struct Parameter* next;
};
// Struct declarations
struct packet {
    uint8_t type;
    uint8_t Status;
    unsigned long payload_size;
    struct Parameter * payload;
};

//
struct Parameter* createParameter( char* data);
void appendParameter(struct Parameter* payload, char* data);

// Simple chained list containing item needed to be sent.
//Function Declarations
//These manage the packet struct
void destroy_packet(struct packet *);
void deletePayload(struct Parameter**);
struct packet *deserialize_payload(const char *);

//These wrapper functions send and receive the payload struct
//-1 for error 0 for success
int send_packet(int,struct packet *);
struct packet * recv_packet(int);

#endif // PROTOCOL_LIB_H