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
struct payload {
    uint8_t type;
    uint8_t Code;
    uint8_t Status;
    uint16_t payload_size;
    char* payload;
};


//Function Declarations
//These manage the payload struct
void destroy_payload(struct payload *);
char *serialize_payload(const struct payload *);
struct payload *deserialize_payload(const char *);

//These wrapper functions send and receive the payload struct
//-1 for error 0 for success
int send_payload(int, struct payload *);
payload * recv_payload(int);

#endif // PROTOCOL_LIB_H