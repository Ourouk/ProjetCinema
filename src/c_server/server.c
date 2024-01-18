#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <string.h>
#include <openssl/sha.h>
#include <errno.h>

//Import the protocol library
#include "../protocol_lib/protocol_lib.h"

//Import the database library
#include "sqlite-amalgamation/sqlite3.h"
#include <sys/select.h>

//Define Server Varibles
#define IP_ADDRESS 0 //Configured to listen on all interfaces
#define IP_PORT 5050
#define SQLITE3_MAX_LENGTH 1000
//Define server handling of each client
void *handle_client(void *) ;

// Define the servers commands handlers
void handle_get_movie_list(int);
void handle_get_shows(int,struct packet*);
uint8_t handle_login(int,struct packet*);
uint8_t handle_logout(int);
void handle_reserve_seats(int,struct packet*);
void handle_add_movie(int,struct packet*);
void handle_add_show(int,struct packet*);
void handle_encryption_status(int ,struct packet*);

/*TOOLS*/
//Database pre-made tools
int open_database(const char* path);
void init_table(sqlite3 *db);
//Encryption
void hash_password(const char *, char *);
void create_account(const char *, const char *);
int login(const char *, const char *);






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
    server_addr.sin_port = htons(IP_PORT    ); // Choose a port (in this example, 8080)
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
/*THREADED PART*/

//Note the technical choice to choose sqlite3 as a database limit the use of mutex sadly...
sqlite3 *db;
pthread_key_t thread_specific_db;
pthread_key_t thread_specific_encryption_flag;
pthread_mutex_t password_mutex; // Declare a mutex variable

void cleanup(void *value) {
    // Cleanup function called when the thread exits
    printf("Cleaning up thread-specific data\n");
    // Perform ,struct packet* payload_buffany necessary cleanup for the specific data
    sqlite3_close(value); // For example, free allocated memory
}
void initialize_thread_specific_data() {
    pthread_key_create(&thread_specific_db,cleanup);
    pthread_key_create(&thread_specific_encryption_flag,cleanup);
}



void *handle_client(void *arg) {
    int client_socket = *((int *)arg);
    char connected = 1;
    //Open the database and configure it if needed
    open_database("data/sqlite.db");
    init_table(db);
    u_int8_t connected_flag = 1;
    u_int8_t * encryption_flag = malloc(sizeof(u_int8_t));
    *encryption_flag = 0; //default state
    pthread_setspecific(thread_specific_encryption_flag, encryption_flag);
    while(connected)
    {
        struct packet*  payload_buff = recv_packet(client_socket,pthread_getspecific(thread_specific_encryption_flag));
        //Handle Command
        if(payload_buff == NULL)
        {
            printf("Client shut down communication or an error occurred. Exiting...\n");
            break;
        }else
        {
            u_int8_t general_error_flag = 0;
            switch (payload_buff->type)
            {
                case 0x01: // Get movie list
                    handle_get_movie_list(client_socket);
                    break;
                case 0x02: // Get available shows for a Movie
                    handle_get_shows(client_socket,payload_buff);
                    break;
                case 0x03: // Reserve x seats for a movie
                    if(connected_flag)
                        handle_reserve_seats(client_socket,payload_buff);
                    else
                        general_error_flag = 1;
                    break;
                case 0x04: //Add a Movie
                    if(connected_flag)
                        handle_add_movie(client_socket,payload_buff);
                    else
                        general_error_flag = 1;
                    break;
                case 0x05: //Add a Show
                    if(connected_flag)
                        handle_add_show(client_socket,payload_buff);
                    else
                        general_error_flag = 1;
                    break;
                case 0x06: //Login
                        connected_flag = handle_login(client_socket,payload_buff);
                    break;
                case 0x7:  //Logout
                    if(connected_flag)
                        connected_flag = handle_logout(client_socket);
                    else
                        general_error_flag = 1;
                    break;
                case 0x08: //Encryption status
                        handle_encryption_status(client_socket,payload_buff);
                    break;
                default:
                    /* code pour gérer une demande inconnue */
                    general_error_flag = 1;
                    break;
            }
            if(general_error_flag)
            {
                struct packet* packet_buff = malloc(sizeof(struct packet));
                packet_buff->type = 0x88;
                packet_buff->Status = 0x00;
                packet_buff->payload = NULL;
                send_packet(client_socket,packet_buff,pthread_getspecific(thread_specific_encryption_flag));
                free(packet_buff);
            }
        }
        //Clear mallocs after each command
        deletePayload(&payload_buff->payload);
        free(payload_buff);
    }
    free(encryption_flag);
    close(client_socket);
    pthread_exit(NULL);
}

// Define the servers commands handlers
void handle_get_movie_list(int client_socket) {
    int rc;
    sqlite3 * db = pthread_getspecific(thread_specific_db);
    sqlite3_stmt *stmt;
    const char *sql = "SELECT movie_id, title FROM Movies;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    size_t counter = 0;
    struct Parameter* parameter_buff = NULL;
    int column_count = sqlite3_column_count(stmt);
    u_int8_t first_run_flag = 1;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        for (int i = 0; i < column_count; i++)
        {
            if(first_run_flag){
                parameter_buff = createParameter((char*)sqlite3_column_text(stmt, i));
                first_run_flag = 0;
            }
            else{
                const unsigned char * val = sqlite3_column_text(stmt, i);
                appendParameter(parameter_buff,val);
            }
        }
    }
    struct packet* packet_buff = malloc(sizeof(struct packet));
    packet_buff->type = 0x81;
    packet_buff->Status = 0x01;
    packet_buff->payload = parameter_buff;
    send_packet(client_socket,packet_buff,pthread_getspecific(thread_specific_encryption_flag));
    deletePayload(&packet_buff->payload);
    free(packet_buff);
}
void handle_get_shows(int client_socket,struct packet* payload_buff) {
    int rc;
    sqlite3 * db = pthread_getspecific(thread_specific_db);
    sqlite3_stmt *stmt;
    payload_buff->payload->data[5];
    char *sql = malloc(sizeof("SELECT show_id, nbr_seats, start_time, end_time, show_date FROM Shows WHERE movie_id = cccc;"));
    if(sql == NULL)
    {
        printf("An error occurred. Exiting...\n");
        exit(EXIT_FAILURE);
    }
    sprintf(sql,"SELECT show_id, nbr_seats, start_time, end_time, show_date FROM Shows WHERE movie_id = %4s;",payload_buff->payload->data); 
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_int(stmt, 1, atoi(payload_buff->payload->data));
    size_t counter = 0;
    struct Parameter* parameter_buff = NULL;
    int column_count = sqlite3_column_count(stmt);
    u_int8_t first_run_flag = 1;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        for (int i = 0; i < column_count; i++)
        {
            if(first_run_flag){
                parameter_buff = createParameter((char*)sqlite3_column_text(stmt, i));
                first_run_flag = 0;
            }
            else{
                const unsigned char * val = sqlite3_column_text(stmt, i);
                appendParameter(parameter_buff,val);
            }
        }
    }
    free(sql);
    struct packet* packet_buff = malloc(sizeof(struct packet));
    packet_buff->type = 0x82;
    packet_buff->Status = 0x01;
    packet_buff->payload = parameter_buff;
    send_packet(client_socket,packet_buff,pthread_getspecific(thread_specific_encryption_flag));
    deletePayload(&packet_buff->payload);
    free(packet_buff);
}
uint8_t handle_login(int client_socket,struct packet* payload_buff)
{
    char * username = payload_buff->payload->data;
    char * password = payload_buff->payload->next->data;
    if(login(username, password))
    {
        struct packet* packet_buff = malloc(sizeof(struct packet));
        packet_buff->type = 0x83;
        packet_buff->Status = 0x01;
        packet_buff->payload = NULL;
        send_packet(client_socket,packet_buff,pthread_getspecific(thread_specific_encryption_flag));
        return 1;
    }
    else
    {
        struct packet* packet_buff = malloc(sizeof(struct packet));
        packet_buff->type = 0x83;
        packet_buff->Status = 0x00;
        packet_buff->payload = NULL;
        send_packet(client_socket,packet_buff,pthread_getspecific(thread_specific_encryption_flag));
        return 0;
    }
}
uint8_t handle_logout(int client_socket)
{
    struct packet* packet_buff  = malloc(sizeof(struct packet));
    packet_buff->type = 0x84;
    packet_buff->Status = 0x01;
    packet_buff->payload = NULL;
    send_packet(client_socket,packet_buff,pthread_getspecific(thread_specific_encryption_flag));
    free(packet_buff);
    return 0;
}
void handle_reserve_seats(int client_socket,struct packet* payload_buff)
{
    int rc;
    sqlite3 * db = pthread_getspecific(thread_specific_db);
    sqlite3_stmt *stmt;
    //INSERT INTO Movies (title, genre, director, release_date)VALUES ('Inception', 'Science Fiction', 'Christopher Nolan', '2010-07-16')
    char sql[SQLITE3_MAX_LENGTH];
    //It's horrible
    sprintf(sql,"UPDATE SHOWS SET nbr_seats = nbr_seats - %s WHERE show_id = %s;",payload_buff->payload->next->data,payload_buff->payload->data);
    rc =  sqlite3_prepare_v2(db,sql, -1, &stmt, NULL);
    struct packet* packet_buff = malloc(sizeof(struct packet));
    packet_buff->type = 0x86;
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        packet_buff->Status = 0x00;
    }else{
        packet_buff->Status = 0x01;
    }
    send_packet(client_socket,packet_buff,pthread_getspecific(thread_specific_encryption_flag));
    deletePayload(&packet_buff->payload);
    free(packet_buff);
}
void handle_add_movie(int client_socket,struct packet* payload_buff)
{
    int rc;
    sqlite3 * db = pthread_getspecific(thread_specific_db);
    sqlite3_stmt *stmt;
    //INSERT INTO Movies (title, genre, director, release_date)VALUES ('Inception', 'Science Fiction', 'Christopher Nolan', '2010-07-16')
    char sql[SQLITE3_MAX_LENGTH];
    //It's horrible
    sprintf(sql,"INSERT INTO Movies (title, genre, director, release_date)VALUES ('%s', '%s', '%s', '%s');",payload_buff->payload->data,payload_buff->payload->next->data,payload_buff->payload->next->next->data,payload_buff->payload->next->next->next->data);
    rc =  sqlite3_prepare_v2(db,sql, -1, &stmt, NULL);
    struct packet* packet_buff = malloc(sizeof(struct packet));
    packet_buff->type = 0x85;
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        packet_buff->Status = 0x00;
    }else{
        packet_buff->Status = 0x01;
    }
    send_packet(client_socket,packet_buff,pthread_getspecific(thread_specific_encryption_flag));
    deletePayload(&packet_buff->payload);
    free(packet_buff);
}
void handle_add_show(int client_socket,struct packet* payload_buff)
{
    int rc;
    sqlite3 * db = pthread_getspecific(thread_specific_db);
    sqlite3_stmt *stmt;
    //INSERT INTO Shows (movie_id, nbr_seats, start_time, end_time, show_date) VALUES (1, 100, '18:00', '21:00', '2024-01-05'),
    char sql[SQLITE3_MAX_LENGTH];
    //It's horrible
    sprintf(sql,"INSERT INTO Shows (movie_id, nbr_seats, start_time, end_time, show_date) VALUES (%s, %s, '%s', '%s', '%s'),",payload_buff->payload->data,payload_buff->payload->next->data,payload_buff->payload->next->next->data,payload_buff->payload->next->next->next->data,payload_buff->payload->next->next->next->next->data);
    rc =  sqlite3_prepare_v2(db,sql, -1, &stmt, NULL);
    struct packet* packet_buff = malloc(sizeof(struct packet));
    packet_buff->type = 0x87;
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        packet_buff->Status = 0x00;
    }else{
        packet_buff->Status = 0x01;
    }
    send_packet(client_socket,packet_buff,pthread_getspecific(thread_specific_encryption_flag));
    deletePayload(&packet_buff->payload);
    free(packet_buff);
}
void handle_encryption_status(int client_socket,struct packet* payload_buff)
{
    if(payload_buff->Status)
    {
        struct packet* packet_buff = malloc(sizeof(struct packet));
        packet_buff->type = 0x91;
        packet_buff->Status = 0x01;
        packet_buff->payload = NULL;
        send_packet(client_socket,packet_buff,pthread_getspecific(thread_specific_encryption_flag));
        free(packet_buff);
        u_int8_t * encryption_flag = pthread_getspecific(thread_specific_encryption_flag);
        *encryption_flag = 1;
    }else
    {
        u_int8_t * encryption_flag = pthread_getspecific(thread_specific_encryption_flag);
        *encryption_flag = 0;
    }

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
    if (sqlite3_prepare_v2(db, "CREATE TABLE IF NOT EXISTS Movies (movie_id INT PRIMARY KEY AUTOINCREMENT,title VARCHAR(255),genre VARCHAR(100),director VARCHAR(100),release_date DATE);", -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "Already exist\n");
    } else {
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            fprintf(stdout, "Table creation failed: %s\n", sqlite3_errmsg(db));
        } else {
            printf("Table created successfully\n");
        }
    }
    sqlite3_finalize(stmt); // finalize the statement here
    if (sqlite3_prepare_v2(db, "CREATE TABLE IF NOT EXISTS Shows (show_id INT PRIMARY KEY AUTOINCREMENT,movie_id INT,nbr_seats INT,start_time TIME,end_time TIME,show_date DATE,FOREIGN KEY (movie_id) REFERENCES Movies(movie_id));", -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stdout, "Already exist\n");
    } else {
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            fprintf(stdout, "Table creation failed: %s\n", sqlite3_errmsg(db));
        } else {
            printf("Table created successfully\n");
        }
    }
    sqlite3_finalize(stmt); // finalize the second statement here
}


//Account Management
void hash_password(const char *password, char *hashed_password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)password, strlen(password), hash);
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        sprintf(hashed_password + (i * 2), "%02x", hash[i]);
}
//Create hashed csv files
void create_account(const char *username, const char *password) {
    char hashed_password[65];
    hash_password(password, hashed_password);
    FILE *file = fopen("data/passwords.csv", "r");
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char *comma = strchr(line, ',');
        *comma = '\0';
        if (strcmp(line, username) == 0) {
            fclose(file);
            printf("Username already exists.\n");
            return;
        }
    }
    fclose(file);
    file = fopen("data/passwords.csv", "a");
    fprintf(file, "%s,%s\n", username, hashed_password);
    fclose(file);
}

int login(const char *username, const char *password) {
    pthread_mutex_lock(&password_mutex); // Lock the mutex before accessing the file
    char hashed_password[65];
    hash_password(password, hashed_password);
    FILE *file = fopen("data/passwords.csv", "r");
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char *comma = strchr(line, ',');
        char *username_f = strtok(line, ","); // Tokenize string before comma
        char *password_f = strtok(comma + 1, "\n"); // Tokenize string after comma
        if (strcmp(username_f, username) == 0 && strcmp(password_f, hashed_password) == 0) {
            fclose(file);
            pthread_mutex_unlock(&password_mutex); // Unlock the mutex after accessing the file
            return 1;
        }
    }
    fclose(file);
    pthread_mutex_unlock(&password_mutex); // Unlock the mutex after accessing the file
    return 0;
}
