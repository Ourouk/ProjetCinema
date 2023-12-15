#ifndef PROTOCOL_LIB_H
#define PROTOCOL_LIB_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX_PAYLOAD_SIZE 2048

// Struct declarations
struct packet {
    uint8_t type;
    uint8_t Status;
    uint16_t payload_size;
    char* payload;
};

// Simple chained list containing item needed to be sent.
struct Parameter {
    void* data; // This will hold the pointer
    struct Parameter* next;
};

//Function Declarations
//These manage the payload struct
void destroy_packet(struct packet *);
char *serialize_packet(const struct packet *);
struct packet *deserialize_payload(const char *);

//These wrapper functions send and receive the payload struct
//-1 for error 0 for success
int send_packet(int,const struct packet *);
struct packet * recv_packet(int);

#endif // PROTOCOL_LIB_H