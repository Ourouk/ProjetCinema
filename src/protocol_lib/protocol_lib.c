#include "protocol_lib.h"
#include <errno.h>

//MANAGING THE PAYLOAD
// Function to create a new parameter
struct Parameter* createParameter(char* data) {
    struct Parameter* newParameter = (struct Parameter*)malloc(sizeof(struct Parameter));
    if (newParameter == NULL) {
        // Handle error here
        return NULL;
    }
    int sizeofmalloc =  strlen(data) + 1; // +1 for the null terminator
    char* new_data = (char*)malloc(sizeof(char)*sizeofmalloc);
    if (new_data == NULL) {
        // Handle error here
        free(newParameter);
        return NULL;
    }
    strcpy(new_data, data);
    newParameter->data = new_data;
    newParameter->next = NULL;
    return newParameter;
}
// Function to add a new parameter to the end of the payload
void appendParameter(struct Parameter* payload, char* data) {
    struct Parameter* newParameter = createParameter(data);
    if (payload == NULL) {
        payload = newParameter;
        return;
    } else {
        struct Parameter* current = payload;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newParameter;
    }
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
//Managing the packet struct
void destroy_packet(struct packet *msg)
{
    deletePayload(&msg->payload);
    free(msg);
}
int send_packet(int socket, struct packet *msg)
{
    fprintf(stdout, "Begin Network Log for send_packet\n");

    // Serialize the packet
    void *buffer_payload = NULL;
    size_t string_size = 0;
    struct Parameter *param = msg->payload;
    //Parse the parameter payload into a string
    if (param != NULL) {
        u_int8_t count = 0;
        string_size = strlen(param->data) + 1; // +1 for the null terminator
        char * string_buffer = calloc(string_size,sizeof(char));
        struct Parameter *current = param;
        u_int8_t continue_flag = 1;
        while (continue_flag)
        {
            if (count != 0)
            {
                strcat(string_buffer, ","); // \0 is already in the string_buffer
                string_size += strlen(current->data) + 1; // +1 for the comma
                string_buffer = realloc(string_buffer, string_size);
            }
            strcat(string_buffer, current->data);
            if(current->next == NULL)
                continue_flag = 0;
            else
                current = current->next;
            count++;
        }
        buffer_payload = string_buffer;
    }else
    {
        buffer_payload = NULL;
    }
    msg->payload_size = string_size;
    // Sending the packet
    // char *packet_buffer = malloc(sizeof(struct packet));
    // memcpy(packet_buffer, msg, sizeof(struct packet));

    //Reduce the size of the packet to the header size
    size_t packetHeader_size = sizeof(u_int8_t)+sizeof(u_int8_t)+sizeof(size_t);
    void * packet_buffer = malloc(packetHeader_size);
    if (packet_buffer == NULL) {
        perror("Error allocating memory for packet");
        return -1;
    }
    memcpy(packet_buffer,&(msg->type), sizeof(u_int8_t));
    memcpy(packet_buffer + sizeof(u_int8_t),&(msg->Status),  sizeof(u_int8_t));
    memcpy(packet_buffer + sizeof(u_int8_t) + sizeof(u_int8_t),&(msg->payload_size),sizeof(size_t));

    fprintf(stdout, "Packet Header : Type : %x, Status : %d, Payload Size : %zu\n", msg->type, msg->Status, msg->payload_size);
    fflush(stdout);
    int bytes_sent = send(socket, packet_buffer, packetHeader_size, 0);
    fprintf(stdout, "Bytes sent: %d\n", bytes_sent);
    if (bytes_sent == -1) {
        perror("Error sending header data");
        free(packet_buffer);
        return -1;
    }

    if (msg->payload_size != 0 && buffer_payload != NULL) {
        bytes_sent = send(socket, buffer_payload, msg->payload_size, 0);
        fprintf(stdout, "Payload: %s\n", (char*)buffer_payload);
        fprintf(stdout, "Payload's bytes sent: %d\n", bytes_sent);
        if (bytes_sent == -1) {
            perror("Error sending payload data");
            free(packet_buffer);
            return -1;
        }
    }
    free(packet_buffer);
    free(buffer_payload);
    fprintf(stdout, "Ending Network Log for send_packet\n\n");
    return 0;
}
//TODO: If a socket error happen automaticly close the socket and close the thread
struct packet *recv_packet(int socket) {
    fprintf(stdout, "Begin Network Log for recv_packet\n");
    struct packet_header {
        uint8_t type;
        uint8_t Status;
        size_t payload_size;
    };
    size_t packetHeader_size = sizeof(u_int8_t)+sizeof(u_int8_t)+sizeof(size_t);
    void * smallHeader_buffer = malloc(packetHeader_size);
    // struct packet_header *header_buffer = malloc(sizeof(struct packet_header));
    if (smallHeader_buffer == NULL) {
        perror("Error allocating memory for header");
        return NULL;
    }
    // memset(header_buffer, 0, sizeof(struct packet_header));
    unsigned long bytes_received = 0;
    while (bytes_received < packetHeader_size) {
        ssize_t result = recv(socket, smallHeader_buffer + bytes_received, packetHeader_size - bytes_received, 0);
        if (result <= 0) {
            fprintf(stderr, "Error receiving header data %s\n", strerror(errno));
            free(smallHeader_buffer);
            close(socket);
            return NULL;
        }
        bytes_received += result;
    }
    struct packet_header * header_buffer = malloc(sizeof(struct packet_header));
    header_buffer->type = *(uint8_t*)smallHeader_buffer;
    header_buffer->Status = *(uint8_t*)(smallHeader_buffer + sizeof(u_int8_t));
    header_buffer->payload_size = *(size_t*)(smallHeader_buffer + sizeof(u_int8_t) + sizeof(u_int8_t));
    free(smallHeader_buffer);
    if (header_buffer == NULL) {
        perror("Error allocating memory for header");
        return NULL;
    }

    fprintf(stdout, "Packet Header : Type : %x, Status : %d, Payload Size : %zu\n", header_buffer->type, header_buffer->Status, header_buffer->payload_size);
    fprintf(stdout, "Header's bytes received: %lu\n", bytes_received);
    bytes_received = 0;
    struct Parameter *param = NULL;
    if (header_buffer->payload_size != 0) {
        char *payload = malloc(header_buffer->payload_size + 1);
        if (payload == NULL) {
            perror("Error allocating memory for payload");
            free(header_buffer);
            return NULL;
        }
        memset(payload, 0, header_buffer->payload_size + 1);
        while (bytes_received < header_buffer->payload_size) {
            ssize_t result = recv(socket, payload + bytes_received, header_buffer->payload_size - bytes_received, 0);

            if (result <= 0) {
                perror("Error receiving payload data");
                free(payload);
                free(header_buffer);
                return NULL;
            }

            bytes_received += result;
        }

        fprintf(stdout, "Payload: %s\n", payload);
        fprintf(stdout, "Payload's bytes received: %lu\n", bytes_received);
        if (payload == NULL || payload[0] == '\0')
        {
            param = NULL;
        }else
        {
            char **saveptr = NULL; // Remove 'restrict' since it's not necessary here
            char *token = strtok_r(payload, ",", &saveptr);
            if (token == NULL) {
                // Handle case when no token is found
                param = NULL;
            }else
            {
                struct Parameter *head_param = createParameter(token); // Assuming createParameter initializes a list
                if (head_param == NULL) {
                    // Handle memory allocation failure
                    param = NULL;
                }else
                {
                    while ((token = strtok_r(NULL, ",", &saveptr)) != NULL) {
                        appendParameter(head_param, token); // Assuming appendParameter appends to the list
                    }
                    param = head_param;
                }

            }
        }
        free(payload); //strtok_r just stores pointers from the original maloc
    }

    struct packet *msg = malloc(sizeof(struct packet));

    if (msg == NULL) {
        perror("Error allocating memory for packet");
        free(header_buffer);
        return NULL;
    }

    msg->type = header_buffer->type;
    msg->Status = header_buffer->Status;
    msg->payload_size = header_buffer->payload_size;
    free(header_buffer);
    msg->payload = param;

    fprintf(stdout, "Ending Network Log for recv_packet\n\n");

    if (msg == NULL) {
        printf("An error occurred. Exiting...\n");
        exit(EXIT_FAILURE);
    }

    return msg;
}