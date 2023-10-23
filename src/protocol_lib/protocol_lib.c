#include "protocol_lib.h"

struct payload *create_payload(const uint8_t type,const uint8_t Code,const uint8_t Status,const uint16_t payload_size,const char *payload)
{
    struct payload *msg = malloc(sizeof(struct payload));
    msg->type = type;
    msg->Code = Code;
    msg->Status = Status;
    msg->payload_size = payload_size;
    msg->payload = payload; 
    return msg;
}

void destroy_payload(struct payload *msg)
{
    if (msg->payload != NULL) {
        free(msg->payload);
    }
}
//Note should execute a destroy payload after this
char *serialize_payload(const struct payload *msg)
{
    char *buffer = malloc(sizeof(sizeof(struct payload)- sizeof(char *)+msg->payload_size));
    memcpy(buffer, msg, sizeof(struct payload)- sizeof(char *));
    memcpy(buffer + sizeof(struct payload)- sizeof(char *), msg->payload, msg->payload_size);
    return buffer;
}

struct payload *deserialize_payload(const char *buffer)
{
    return create_payload(buffer[0], buffer[1], buffer[2], (buffer[3] << 8) | buffer[4],buffer[5]);
}

int send_payload(int socket,struct payload *msg)
{
    char *buffer = serialize_payload(msg);
    int bytes_sent = send(socket, buffer,sizeof(sizeof(struct payload)- sizeof(char *)+msg->payload_size) , 0);
    if(bytes_sent == -1)
    {
        perror("Error sending data");
        return -1;
    }else
    {
        return 0;
    }
}

struct payload *recv_payload(int socket)
{
    char header_buffer[sizeof(struct payload)- sizeof(char *)];
    recv(socket, header_buffer, sizeof(u_int8_t), 0);
    recv(socket, header_buffer+sizeof(u_int8_t), sizeof(u_int8_t), 0);
    recv(socket, header_buffer+sizeof(u_int8_t)*2, sizeof(u_int8_t), 0);
    recv(socket, header_buffer+sizeof(u_int8_t)*3, sizeof(u_int16_t), 0);
    char *payload = malloc((header_buffer[3] << 8) | header_buffer[4]);
    recv(socket, payload+sizeof(u_int8_t)*5, (header_buffer[3] << 8) | header_buffer[4], 0);
    return create_payload(header_buffer[0], header_buffer[1], header_buffer[2], (header_buffer[3] << 8) | header_buffer[4],payload);
}
    
