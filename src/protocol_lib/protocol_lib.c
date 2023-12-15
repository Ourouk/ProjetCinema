#include "protocol_lib.h"



// Function to create a new parameter
struct Parameter* createParameter(void* data) {
    struct Parameter* newParameter = (struct Parameter*)malloc(sizeof(struct Parameter));
    if(newParameter == NULL) {
        // Handle error here
        return NULL;
    }
    newParameter->data = data;
    newParameter->next = NULL;
    return newParameter;
}

// Function to add a new parameter to the end of the payload
void appendParameter(struct Parameter** payload, void* data) {
    struct Parameter* newParameter = createParameter(data);
    if(*payload == NULL) {
        *payload = newParameter;
        return;
    }
    struct Parameter* current = *payload;
    while(current->next != NULL) {
        current = current->next;
    }
    current->next = newParameter;
}

// Function to delete the entire payload
void deletePayload(struct Parameter** payload) {
    struct Parameter* current = *payload;
    struct Parameter* next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    *payload = NULL;
}

struct packet *create_packet(const uint8_t type,const uint8_t Status,const uint16_t payload_size,const char *payload)
{
    struct packet *msg = malloc(sizeof(struct packet));
    msg->type = type;
    msg->Status = Status;
    msg->payload_size = payload_size;
    msg->payload = NULL;
    memcpy(msg->payload,payload,sizeof(char)*payload_size);
    return msg;
}

void destroy_packet(struct packet *msg)
{
    if (msg->payload != NULL) {
        free(msg->payload);
    }
}
//Note should execute a destroy payload after this
char *serialize_packet(const struct packet *msg)
{
    char *buffer = malloc(sizeof(sizeof(struct packet)- sizeof(char *)+msg->payload_size));
    memcpy(buffer, msg, sizeof(struct packet)- sizeof(char *));
    memcpy(buffer + sizeof(struct packet)- sizeof(char *), msg->payload, msg->payload_size);
    return buffer;
}

int send_packet(int socket,const struct packet *msg)
{
    char *buffer = serialize_packet(msg);
    int bytes_sent = send(socket, buffer,sizeof(sizeof(struct packet)- sizeof(char *)+msg->payload_size) , 0);
    if(bytes_sent == -1)
    {
        perror("Error sending data");
        return -1;
    }else
    {
        return 0;
    }
}

struct packet *recv_packet(int socket)
{
    char header_buffer[sizeof(struct packet)- sizeof(char *)];
    unsigned long bytes_received = 0;
    while(bytes_received < sizeof(header_buffer))
    {
        ssize_t result = recv(socket, header_buffer + bytes_received, sizeof(header_buffer) - bytes_received, 0);
        if (result <= 0) {
            // Handle error or closure and possibly return
            return NULL;
        }
        bytes_received += result;
    }
    bytes_received = 0;
    uint16_t payload_size = (header_buffer[3] << 8) | header_buffer[4];
    char *payload = malloc(payload_size);
    if (payload == NULL) {
        // Handle error: malloc failed
        return NULL;
    }
    while (bytes_received < payload_size)
    {
        ssize_t result = recv(socket, payload, payload_size-bytes_received, 0);
        if (result <= 0) {
            return NULL;
        }
        bytes_received += result;
    }
    return create_packet(header_buffer[0], header_buffer[1], payload_size,payload);
}