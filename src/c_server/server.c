#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <string.h>

//Import the protocol library
#include "../protocol_lib/protocol_lib.h"

//Import the database library
#include "sqlite-amalgamation/sqlite3.h"

//Define server handling of each client
void *handle_client(void *) ;
//Database pre-made tools
int open_database(const char* path);
void init_table(sqlite3 *db);
// Define the servers commands handlers
void handle_get_movie_list(int);
void handle_get_available_seats(int,struct packet*);
void handle_auth_admin(int,struct packet*);
void handle_reserve_seats(int,struct packet*);
void handle_add_movie(int,struct packet*);
void handle_add_show(int,struct packet*);


//Note the technical choice to choose sqlite3 as a database limit the use of mutex sadly...
sqlite3 *db;
pthread_key_t thread_specific_db;
void cleanup(void *value) {
    // Cleanup function called when the thread exits
    printf("Cleaning up thread-specific data\n");
    // Perform any necessary cleanup for the specific data
    sqlite3_close(value); // For example, free allocated memory
}
void initialize_thread_specific_data() {
    pthread_key_create(&thread_specific_db,cleanup);
}
// Warning do not strtok but strtok_r because static variable would cause problems
int main() {
    // Create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Define the server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5050); // Choose a port (in this example, 8080)
    server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on any available interface

    // Bind the socket to the specified address
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(sockfd, 10) == -1) { // The second argument is the backlog queue size
        perror("listen");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    // Accept incoming connections
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    //Enable multi-threading on sqlite3
    sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
    initialize_thread_specific_data();
    while(1)
    {
        int client_sockfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
        if (client_sockfd == -1) {
            perror("accept");
            close(sockfd);
            exit(EXIT_FAILURE);
        }else
        {
            //At demand Handling
            printf("Client connected\n");
            pthread_t handle_client_thread;
            int result = pthread_create(&handle_client_thread, NULL, handle_client, &client_sockfd);
            if (result != 0) {
                perror("Thread creation failed");
                return 1;
            }
        }
    }
    return 0;
}

void *handle_client(void *arg) {
    int client_socket = *((int *)arg);
    char connected = 1;
    //Open the database and configure it if needed
    open_database("data/sqlite.db");
    init_table(db);
    while(connected)
    {
        // Wait to receive the message
        struct packet* payload_buff = recv_packet(client_socket);
        //Handle Command
        if(payload_buff == NULL)
        {
            printf("An error occurred. Exiting...\n");
            exit(EXIT_FAILURE);
        }
        switch (payload_buff->type)
        {
            case 0x01: // Get movie list
                handle_get_movie_list(client_socket);
                break;
            case 0x02: // Get available seats for a show
                //handle_get_available_seats(client_socket,payload_buff);
                break;
            case 0x03: // Auth to admin mode
                //handle_auth_admin(client_socket,payload_buff);
                break;
            case 0x04: // Reserve x seats for a movie
                //handle_reserve_seats(client_socket,payload_buff);
                break;
            case 0x05: // Add a movie
                //handle_add_movie(client_socket,payload_buff);
                break;
            case 0x06: // Add a movie max number of seat
                //handle_add_show(client_socket,payload_buff);
                break;
            default:
                /* code pour gérer une demande inconnue */
                break;
        }
    }
    close(client_socket);
    pthread_exit(NULL);
}

// Define the servers commands handlers
void handle_get_movie_list(int client_socket) {
    int rc;
    sqlite3 * db = pthread_getspecific(thread_specific_db);
    sqlite3_stmt *stmt;
    const char *sql = "SELECT title, genre, director,release_date FROM Movies;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    size_t counter = 0;
    struct Parameter* parameter_buff;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        if(counter == 0)
        {
            const  char* column_text = (const char*)sqlite3_column_text(stmt, 0);
            char* new_pointer = malloc(strlen(column_text) + 1);
            if (new_pointer != NULL) {
                strcpy(new_pointer, column_text);
               parameter_buff = createParameter(new_pointer);
            }
            
        }else
        {
            const char* column_text = (const char*)sqlite3_column_text(stmt, 0);
            char* new_pointer = malloc(strlen(column_text) + 1);
            if (new_pointer != NULL) {
                strcpy(new_pointer, column_text);
                appendParameter(parameter_buff, new_pointer);
            }
        }
        counter++;
    }
    struct packet* packet_buff = malloc(sizeof(struct packet));
    packet_buff->type = 0x81;
    packet_buff->Status = 0x01;
    packet_buff->payload = parameter_buff;
    send_packet(client_socket,packet_buff);
    destroy_packet(packet_buff);
}

//Database Management
//Database pre-made tools
int open_database(const char* path){
    int rc;
    sqlite3 *db;
    rc = sqlite3_open_v2(path, &db,SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,NULL);
    //Check if the database structure is already created
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(0);
    } else {
        pthread_setspecific(thread_specific_db, db);
        return(1);
    }
}
//Database pre-made tools
void init_table(sqlite3 *db) {
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, "CREATE TABLE IF NOT EXISTS Movies (movie_id INT PRIMARY KEY,title VARCHAR(255),genre VARCHAR(100),director VARCHAR(100),release_date DATE);", -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Already exist\n");
    } else {
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            fprintf(stderr, "Table creation failed: %s\n", sqlite3_errmsg(db));
        } else {
            printf("Table created successfully\n");
        }
    }
    sqlite3_finalize(stmt); // finalize the statement here

    if (sqlite3_prepare_v2(db, "CREATE TABLE IF NOT EXISTS Shows (show_id INT PRIMARY KEY,movie_id INT,nbr_seats INT,start_time TIME,end_time TIME,show_date DATE,FOREIGN KEY (movie_id) REFERENCES Movies(movie_id));", -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Already exist\n");
    } else {
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            fprintf(stderr, "Table creation failed: %s\n", sqlite3_errmsg(db));
        } else {
            printf("Table created successfully\n");
        }
    }
    sqlite3_finalize(stmt); // finalize the second statement here
}
