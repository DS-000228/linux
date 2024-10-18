#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


#define PORT 9999

int main() {
    
    int sock;
    struct sockaddr_in server;
    char message[1000], server_reply[2000];

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    // Connect to remote server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect failed. Error");
        return 1;
    }
     
    puts("Connected\n");

    // Keep communicating with server
    while(1) {
        printf("Enter message: ");
        scanf("%s", message);
        
        // Send some data
        if (send(sock, message, strlen(message), 0) < 0) {
            puts("Send failed");
            return 1;
        }
    }

    close(sock);
    return 0;
}

