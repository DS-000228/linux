#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 23            // 目标地址端口号
#define ADDR "192.168.1.230" // 目标地址IP

int main() {
    int iSocketFD = 0; // socket句柄
    unsigned int iRemoteAddr = 0;
    struct sockaddr_in stRemoteAddr = {0}; // 对端，即目标地址信息
    char buf[4096] = {0}; // 存储接收到的数据
    char input[4096] = {0}; // 存储用户输入的数据

    // 创建socket
    iSocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (0 > iSocketFD) {
        printf("创建socket失败！\n");
        return 0;
    }

    // 设置目标地址
    stRemoteAddr.sin_family = AF_INET;
    stRemoteAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, ADDR, &iRemoteAddr);
    stRemoteAddr.sin_addr.s_addr = iRemoteAddr;

    // 连接
    if (0 > connect(iSocketFD, (struct sockaddr *)&stRemoteAddr, sizeof(stRemoteAddr))) {
        printf("连接失败！\n");
    } else {
        printf("连接成功！\n");

        // 循环接收和发送数据
        while (1) {
            printf("输入数据发送到服务器（输入exit退出）：\n");
            fgets(input, sizeof(input), stdin); // 从控制台读取输入
            // 去掉换行符
            input[strcspn(input, "\n")] = 0;

            // 发送数据
            send(iSocketFD, input, strlen(input), 0);

            // 检查是否要退出
            if (strcmp(input, "exit") == 0) {
                break;
            }

            // 接收服务器的反馈
            int recv_len = recv(iSocketFD, buf, sizeof(buf), 0);
            if (recv_len > 0) {
                buf[recv_len] = '\0'; // 添加字符串结束符
                printf("Received from server: %s\n", buf);
            } else {
                printf("接收失败或服务器关闭连接！\n");
                break;
            }
        }
    }

    close(iSocketFD); // 关闭socket
    return 0;
}

