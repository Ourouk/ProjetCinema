#include "protocol_lib.h"

//MANAGING THE PAYLOAD
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
    void appendParameter(struct Parameter * payload,void* data) {
        struct Parameter* newParameter = createParameter(data);
        if(payload == NULL) {
            payload = newParameter;
            return;
        }else
        {
            struct Parameter* current = payload;
            while(current->next != NULL) {
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

    //Packet's payload parser Parser
    struct Parameter* payload_parser(char * string_received)
    {
        if (string_received == NULL || string_received[0] == '\0') {
            return NULL;
        }

        char **saveptr = NULL; // Remove 'restrict' since it's not necessary here
        char *token = strtok_r(string_received, ",", &saveptr);
        
        if (token == NULL) {
            // Handle case when no token is found
            return NULL;
        }

        struct Parameter *head_param = createParameter(token); // Assuming createParameter initializes a list
        if (head_param == NULL) {
            // Handle memory allocation failure
            return NULL;
        }

        while ((token = strtok_r(NULL, ",", &saveptr)) != NULL) {
            appendParameter(head_param, token); // Assuming appendParameter appends to the list
        }

        return head_param;
        }
        
    //Managing the packet struct
    void destroy_packet(struct packet *msg)
    {
        if (msg->payload != NULL) {
            free(msg->payload);
        }
    }
    //Network stack
    // int send_packet(int socket,struct packet *msg)
    // {
    //     fprintf(stdout, "Begin Network Log for send_packet\n");

    //     //Serialize the packet

    //     void * buffer_payload;
    //     size_t string_size = 0;
    //     struct Parameter * param = msg->payload;
    //     if(param == NULL)
    //     {
    //         buffer_payload = NULL;
    //     }else
    //     {
    //         u_int8_t count = 0;
    //         string_size = strlen(param->data) + 1; // +1 for the null terminator
    //         char * buffer = calloc(string_size,sizeof(char));
    //         struct Parameter *current = param;
    //         while (current->next != NULL)
    //         {
    //             if (count != 0)
    //             {
    //                 strcat(buffer, ","); // \0 is already in the buffer
    //                 string_size += strlen(current->data) + 1; // +1 for the comma
    //                 buffer = realloc(buffer, string_size);
    //             }
    //             strcat(buffer, current->data);
    //             current = current->next;
    //             count++;
    //         }
    //         buffer_payload = buffer;
    //     }
    //     msg->payload_size = string_size;

    //     //send the packet

    //     char *buffer = malloc(sizeof(struct packet)- sizeof(char *));
    //     memcpy(buffer, msg, sizeof(struct packet)- sizeof(char *));
    //     if(msg->payload_size != 0)
    //     {
    //         fprintf(stdout,"Packet Header : Type : %x, Status : %d,Payload strlen : %d , Payload Size : %zu\n",msg->type,msg->Status,strlen(buffer_payload),msg->payload_size);fflush(stdout);
    //     }else
    //     {
    //         fprintf(stdout,"Packet Header : Type : %x, Status : %d, Payload Size : %zu\n",msg->type,msg->Status,msg->payload_size);fflush(stdout);
    //     }
        
    //     int bytes_sent = send(socket, buffer,sizeof(struct packet)- sizeof(char *) , 0);
    //     fprintf(stdout, "Bytes sent: %d\n", bytes_sent);
    //     if(bytes_sent == -1)
    //     {
    //         perror("Error sending data");
    //         return -1;
    //     }else if(msg->payload_size != 0)
    //     {
    //         fprintf(stdout, "Payload: %s\n", buffer_payload);fflush(stdout);
    //         bytes_sent = send(socket, buffer_payload, msg->payload_size, 0);
    //         fprintf(stdout, "Payload's bytes sent: %d\n", bytes_sent);
    //         if(bytes_sent == -1)
    //         {
    //             perror("Error sending data");
    //             return -1;
    //         }
    //     }
    //     free(buffer);
    //     free(buffer_payload);
    //     fprintf(stdout, "Ending Network Log for send_packet \n\n");
    //     return 0;
    // }

    // struct packet *recv_packet(int socket)
    // {
    //     fprintf(stdout, "Begin Network Log for recv_packet\n");
    //     struct packet_header {
    //         uint8_t type;
    //         uint8_t Status;
    //         size_t payload_size;
    //     }; 
    //     struct packet_header *  header_buffer = malloc(sizeof(struct packet_header));
    //     unsigned long bytes_received = 0;
    //     while(bytes_received < sizeof(struct packet_header))
    //     {
    //         ssize_t result = recv(socket, header_buffer + bytes_received, sizeof(header_buffer) - bytes_received, 0);
    //         if (result <= 0) {
    //             //Handle error or closure and possibly return
    //             return NULL;
    //         }
    //         bytes_received += result;
    //     }
    //     fprintf(stdout,"Packet Header : Type : %x, Status : %d, Payload Size : %zu\n",header_buffer->type,header_buffer->Status,header_buffer->payload_size);
    //     fprintf(stdout, "Header's bytes recieved: %d\n", bytes_received);
    //     bytes_received = 0;
    //     struct Parameter * param;
    //     if(header_buffer->payload_size != 0){
    //         char *payload = malloc(header_buffer->payload_size);
    //         if (payload == NULL) {
    //             //Handle error: malloc failed
    //             return NULL;
    //         }
    //         while (bytes_received < header_buffer->payload_size)
    //         {
    //             ssize_t result = recv(socket, payload, header_buffer->payload_size -bytes_received, 0);
    //             if (result <= 0) {
    //                 return NULL;
    //             }
    //             bytes_received += result;
    //         }
    //         fprintf(stdout, "Payload: %s\n", payload);
    //         fprintf(stdout, "Payload's bytes recieved: %d\n", bytes_received);
    //         param = payload_parser(payload);
    //     }else {
    //         param = NULL;
    //     }
    //     struct packet *msg = malloc(sizeof(struct packet));
    //     msg->type = header_buffer->type;
    //     msg->Status = header_buffer->Status;
    //     msg->payload_size = header_buffer->payload_size;
    //     free(header_buffer);
    //     msg->payload = param;
    //     fprintf(stdout, "Ending Network Log for recv_packet \n\n");
    //     if(msg == NULL)
    //     {
    //         printf("An error occurred. Exiting...\n");
    //         exit(EXIT_FAILURE);
    //     }
    //     return msg;
    // }


    int send_packet(int socket, struct packet *msg) {
    fprintf(stdout, "Begin Network Log for send_packet\n");

    // Serialize the packet
    void *buffer_payload = NULL;
    size_t string_size = 0;
    struct Parameter *param = msg->payload;

    if (param != NULL) {
        u_int8_t count = 0;
        string_size = strlen(param->data) + 1; // +1 for the null terminator
        char * buffer = calloc(string_size,sizeof(char));
        struct Parameter *current = param;
        while (current->next != NULL)
        {
            if (count != 0)
            {
                strcat(buffer, ","); // \0 is already in the buffer
                string_size += strlen(current->data) + 1; // +1 for the comma
                buffer = realloc(buffer, string_size);
            }
            strcat(buffer, current->data);
            current = current->next;
            count++;
        }
        buffer_payload = buffer;
    }else
    {
        buffer_payload = NULL;
    }

    msg->payload_size = string_size;

    // Sending the packet
    char *buffer = malloc(sizeof(struct packet));
    memcpy(buffer, msg, sizeof(struct packet));

    fprintf(stdout, "Packet Header : Type : %x, Status : %d, Payload Size : %zu\n", msg->type, msg->Status, msg->payload_size);
    fflush(stdout);

    int bytes_sent = send(socket, buffer, sizeof(struct packet) - sizeof(char *), 0);
    fprintf(stdout, "Bytes sent: %d\n", bytes_sent);

    if (bytes_sent == -1) {
        perror("Error sending header data");
        free(buffer);
        return -1;
    }

    if (msg->payload_size != 0 && buffer_payload != NULL) {
        bytes_sent = send(socket, buffer_payload, msg->payload_size, 0);
        fprintf(stdout, "Payload's bytes sent: %d\n", bytes_sent);

        if (bytes_sent == -1) {
            perror("Error sending payload data");
            free(buffer);
            return -1;
        }
    }

    free(buffer);
    free(buffer_payload);
    fprintf(stdout, "Ending Network Log for send_packet\n\n");
    return 0;
}

struct packet *recv_packet(int socket) {
    fprintf(stdout, "Begin Network Log for recv_packet\n");

    struct packet_header {
        uint8_t type;
        uint8_t Status;
        size_t payload_size;
    };

    struct packet_header *header_buffer = malloc(sizeof(struct packet_header));

    if (header_buffer == NULL) {
        perror("Error allocating memory for header");
        return NULL;
    }

    memset(header_buffer, 0, sizeof(struct packet_header));

    unsigned long bytes_received = 0;

    while (bytes_received < sizeof(struct packet_header)) {
        ssize_t result = recv(socket, header_buffer + bytes_received, sizeof(struct packet_header) - bytes_received, 0);

        if (result <= 0) {
            perror("Error receiving header data");
            free(header_buffer);
            return NULL;
        }

        bytes_received += result;
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

        param = payload_parser(payload);

        free(payload);
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