#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define MAX_CLIENTS 100
#define MAX_MESSAGE_LENGTH 1000

typedef struct {
    int sock;
    char name[MAX_MESSAGE_LENGTH];
} client_t;

client_t clients[MAX_CLIENTS];
pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;

void *handle_client(void *arg) {
    int client_sock = *((int*)arg);
    char buffer[MAX_MESSAGE_LENGTH];
    int n;
    int i;

    
    n = recv(client_sock, buffer, MAX_MESSAGE_LENGTH, 0);
    if (n <= 0) {
        perror("ERROR reading from socket");
        pthread_exit(NULL);
    }
    buffer[n] = '\0';

    
    pthread_mutex_lock(&client_mutex);
    for (i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].sock == 0) {
clients[i].sock = client_sock;
strcpy(clients[i].name, buffer);
break;
}
}
pthread_mutex_unlock(&client_mutex);



memset(buffer, 0, MAX_MESSAGE_LENGTH);
sprintf(buffer, "%s has joined the chat.", clients[i].name);
pthread_mutex_lock(&client_mutex);
     
for (i= 0; i< MAX_CLIENTS; i++) {
    if (clients[i].sock != 0 && clients[i].sock != client_sock) {
        send(clients[i].sock, buffer, strlen(buffer), 0);
    }
}
pthread_mutex_unlock(&client_mutex);

while (1) {
    memset(buffer, 0, MAX_MESSAGE_LENGTH);
    n = recv(client_sock, buffer, MAX_MESSAGE_LENGTH, 0);
    if (n <= 0) {
        break;
    }

    int j;
    for (j = 0; j < MAX_CLIENTS; j++) {
         if (clients[j].sock == client_sock) {
                break;
            }
    }
    char message[1024];
    sprintf(message, "%s:", clients[j].name);
    strcat(message, buffer);
   
   

    pthread_mutex_lock(&client_mutex);
    for (j = 0; j < MAX_CLIENTS; j++) {
        if (clients[j].sock != 0 && clients[j].sock != client_sock) {
            send(clients[j].sock, message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&client_mutex);
}


pthread_mutex_lock(&client_mutex);
for (i = 0; i < MAX_CLIENTS; i++) {
    if (clients[i].sock == client_sock) {
        clients[i].sock = 0;
        break;
    }
}
pthread_mutex_unlock(&client_mutex);


memset(buffer, 0, MAX_MESSAGE_LENGTH);
sprintf(buffer, "%s has left the chat.", clients[i].name);
pthread_mutex_lock(&client_mutex);
for (i = 0; i < MAX_CLIENTS; i++) {
    if (clients[i].sock != 0) {
        send(clients[i].sock, buffer, strlen(buffer), 0);
    }
}
pthread_mutex_unlock(&client_mutex);

close(client_sock);
pthread_exit(NULL);

}

int main(int argc, char *argv[]) {
int sock, client_sock, port, clilen;
struct sockaddr_in server_address, client_address;
pthread_t thread;

if (argc < 2) {
    fprintf(stderr, "ERROR, no port provided\n");
    exit(1);
}

sock = socket(AF_INET,SOCK_STREAM, 0);
if (sock < 0) {
perror("ERROR opening socket");
exit(1);
}

memset(&server_address, 0, sizeof(server_address));

port = atoi(argv[1]);
server_address.sin_family = AF_INET;
server_address.sin_addr.s_addr = INADDR_ANY;
server_address.sin_port = htons(port);


if (bind(sock, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
    perror("ERROR on binding");
    exit(1);
}

listen(sock, MAX_CLIENTS);

memset(clients, 0, sizeof(client_t) * MAX_CLIENTS);

while (1) {
    clilen = sizeof(client_address);
    client_sock = accept(sock, (struct sockaddr *) &client_address, (socklen_t *)&clilen);
    if (client_sock < 0) {
        perror("ERROR on accept");
        exit(1);
    }
    pthread_create(&thread, NULL, handle_client, (void *)&client_sock);
}

close(sock);
return 0;
}




