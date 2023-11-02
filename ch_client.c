#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define MAX_MESSAGE_LENGTH 1000

int sock;

void *send_message(void *arg) {
    char buffer[MAX_MESSAGE_LENGTH];
    int n;

    while (1) {

        memset(buffer, 0, MAX_MESSAGE_LENGTH);
        fgets(buffer, MAX_MESSAGE_LENGTH, stdin);

     
        n = send(sock, buffer, strlen(buffer), 0);
        if (n < 0) {
            perror("ERROR writing to socket");
            exit(1);
        }
    }
}

int main(int argc, char *argv[]) {
    int port;
    struct sockaddr_in server_address;
    char buffer[MAX_MESSAGE_LENGTH];
    int n;
    pthread_t thread;

    if (argc < 3) {
        fprintf(stderr, "ERROR, usage: %s <server IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
if (sock < 0) {
perror("ERROR opening socket");
exit(1);
}


memset(&server_address, 0, sizeof(server_address));
port = atoi(argv[2]);
server_address.sin_family = AF_INET;
server_address.sin_addr.s_addr = inet_addr(argv[1]);
server_address.sin_port = htons(port);
if (connect(sock, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
    perror("ERROR connecting");
    exit(1);
}

printf("Enter your name: ");
fgets(buffer, MAX_MESSAGE_LENGTH, stdin);
buffer[strlen(buffer) - 1] = '\0';

n = send(sock, buffer, strlen(buffer), 0);
if (n < 0) {
    perror("ERROR writing to socket");
    exit(1);
}

pthread_create(&thread, NULL, send_message, NULL);


while (1) {
    memset(buffer, 0, MAX_MESSAGE_LENGTH);
    n = recv(sock, buffer, MAX_MESSAGE_LENGTH, 0);
    if (n <= 0) {
        break;
    }
    printf("%s\n", buffer);
}

close(sock);
return 0;

}


