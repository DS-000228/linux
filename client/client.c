#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char *message;
    char buffer[1024] = {0};

    // 创建 TCP 套接字
    sock = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // 连接服务器
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    // 从控制台读取输入
    printf("Enter message to send: ");
    fgets(buffer, 1024, stdin);

    // 发送数据给服务器
    send(sock, buffer, strlen(buffer), 0);

    // 接收服务器的回应
    read(sock, buffer, 1024);
    printf("Server: %s\n", buffer);

    close(sock);
    return 0;
}

