#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// 定义服务器使用的端口号为 8080。这是客户端将要连接的服务器端口。
#define PORT 8080

int main() {
    int sock = 0; // 定义一个整数 sock 用于存储套接字的文件描述符。
    struct sockaddr_in serv_addr; // 定义一个 sockaddr_in 结构体，存储服务器的 IP 地址和端口号信息。
    char *message;
    char buffer[1024] = {0}; // 定义一个大小为 1024 的字符数组 buffer，用于存储从服务器接收的数据和用户输入的消息

    // 创建 TCP 套接字
    sock = socket(AF_INET, SOCK_STREAM, 0);

    //  配置服务器地址
    serv_addr.sin_family = AF_INET; // 表示地址族为 IPv4
    serv_addr.sin_port = htons(PORT); //将端口号 PORT 转换为网络字节序，存储在 sin_port 字段中

    //  将 IP 地址转换为网络字节序并连接服务器
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    // 从控制台读取最多 1024 字节的输入，存储到 buffer 中
    printf("Enter message to send: ");
    fgets(buffer, 1024, stdin);

    // 将 buffer 中的数据通过套接字 sock 发送到服务器
    send(sock, buffer, strlen(buffer), 0);

    // 从服务器接收数据，存储到 buffer 中，最多接收 1024 字节
    read(sock, buffer, 1024);
    printf("Server: %s\n", buffer); // 打印服务器发送的回应信息

    // 关闭客户端套接字，释放系统资源
    close(sock);
    return 0;
}

