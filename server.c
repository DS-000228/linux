#include <stdio.h>              // 包含标准输入输出头文件，用于输入输出操作
#include <stdlib.h>             // C语言标准库函数的定义
#include <string.h>             // 包含字符串处理函数的头文件
#include <unistd.h>             // 包含POSIX标准API头文件，如读写操作
#include <pthread.h>            // POSIX线程库（Pthreads）的头文件，用于在UNIX和类UNIX系统（如 Linux）中进行多线程编程
#include <netinet/in.h>         // 包含Internet地址族相关的头文件  
#include <sqlite3.h>            // 包含数据库函数的头文件
#include <systemd/sd-bus.h>
#include <errno.h>

//#include "database.h"

#define PORT 9999               // 服务器监听的端口号
#define BUFFER_SIZE 1024
#define BACKLOG 10          // 在listen()调用中允许的最大挂起连接数

// Function declarations
void *handle_client(void *socket_desc);//新线程的入口函数，用于处理单个客户端连接
sd_bus_error error = SD_BUS_ERROR_NULL;
sd_bus_message *m = NULL;
sd_bus *bus = NULL;
const char *path;
int r;


int main() {
    int server_sock, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    //sqlite3 *db=0;
    // Create socket file descriptor
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == 0) { // 创建一个IPv4的TCP套接字
        perror("socket failed");
        return -1;
    }
    
    // Bind the socket to the port 9999
    address.sin_family = AF_INET;// 使用 IPv4 地址
    address.sin_addr.s_addr = INADDR_ANY;// 服务器将接受来自任何网络接口的连接
    address.sin_port = htons(PORT);// 指定服务器监听的端口号
    
    if (bind(server_sock, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        return -1;
    }
    
    // 绑定套接字到指定地址和端口,将一个套接字绑定到一个特定的本地地址和端口号
    if (listen(server_sock, BACKLOG) < 0) {
        perror("listen failed");
        return -1;
    }
    
    printf("Server is up and running on port %d...\n", PORT);

    

    /* Connect to the user bus */
    r = sd_bus_open_user(&bus);
    if (r < 0) {
        fprintf(stderr, "Failed to connect to user bus: %s\n", strerror(-r));
        sd_bus_error_free(&error);
        sd_bus_message_unref(m);
        sd_bus_unref(bus);

        return r < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
    }

    // Accept clients
    while ((new_socket = accept(server_sock, (struct sockaddr *)&address, (socklen_t*)&addrlen))) {
        pthread_t client_thread;
        int *socket_ptr = malloc(sizeof(int));
        *socket_ptr = new_socket;
        // 创建新线程来处理客户端连接
        if(pthread_create(&client_thread, NULL, handle_client, (void*) socket_ptr) < 0) {
            perror("could not create thread");
            return 1;
        }
        pthread_detach(client_thread);
    }

    if (new_socket < 0) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }
    
    return 0;
}

// Handle connection for each client
void *handle_client(void *socket_desc) {
    int sock = *(int*)socket_desc;
    free(socket_desc);
    char buffer[BUFFER_SIZE];
    int read_size;

    //Receive a message from client
    while((read_size = recv(sock, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[read_size] = '\0';
        printf("Received: %s\n", buffer);
        // You can add database insert function call here      
        r = sd_bus_call_method(bus,
                               "net.poettering.dbus",           /* service to contact */
                               "/net/poettering/dbus",          /* object path */
                               "net.poettering.dbus",   /* interface name */
                               "AddMassage",                          /* method name */
                               &error,                               /* object to return error in */
                               &m,                                   /* return message on success */
                               "s",                                /* input signature */
                               buffer);                           
        if (r < 0) {
            fprintf(stderr, "Failed to issue method call: %s\n", error.message);
            sd_bus_error_free(&error);
            sd_bus_message_unref(m);
            sd_bus_unref(bus);

            return r < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
        }

        /* Parse the response message */
        r = sd_bus_message_read(m, "s", &buffer);
        if (r < 0) {
            fprintf(stderr, "Failed to parse response message: %s\n", strerror(-r));
            sd_bus_error_free(&error);
            sd_bus_message_unref(m);
            sd_bus_unref(bus);

            return r < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
        }
	}
    if(read_size == 0) {
        puts("Client disconnected");
    } else if(read_size == -1) {
        perror("recv failed");
    }

    
    return 0;
}
