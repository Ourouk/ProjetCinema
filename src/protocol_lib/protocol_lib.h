#ifndef PROTOCOL_LIB_H
#define PROTOCOL_LIB_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <openssl/aes.h>

#define MAX_PAYLOAD_SIZE 2048

struct Parameter {
    char* data; // This will hold the pointer
    struct Parameter* next;
};
// Struct declarations
struct packet {
    uint8_t type;
    uint8_t Status;
    unsigned int payload_size;
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
int send_packet(int,struct packet *,void * encryption_flag);
struct packet * recv_packet(int,void * encryption_flag);


//Here will be written an heresy
#define AES_DEFAULT_KEY   "htn5CZfrnxvzg6VH";
//Encryption tools
/*Tools used by client server to encrypt payload*/
void encryptAES_ECB(const unsigned char *plaintext, const unsigned char *key, unsigned char *ciphertext);
void decryptAES_ECB(const unsigned char *ciphertext, const unsigned char *key, unsigned char *decryptedtext, unsigned long block_nbr);
#endif // PROTOCOL_LIB_H