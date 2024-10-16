#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sqlite3.h>

#define PORT 8080   // 服务器监听的端口号，设为 8080
#define BUFFER_SIZE 1024  // 缓冲区大小，用于存储从客户端接收到的数据，设为 1024 字节

// 数据库插入函数
void database_insert(sqlite3 *db, const char *data)    // 将客户端发送的数据插入到数据库 server.db 中的 data_table 表里
{
    char *err_msg = 0;
    char sql[256];
    sprintf(sql, "INSERT INTO data_table(content) VALUES('%s');", data);   //生成 SQL 插入语句，将接收到的数据插入到 content 列
    sqlite3_exec(db, sql, 0, 0, &err_msg);   // 执行 SQL 语句，将数据写入数据库
    if (err_msg) {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
}

// 处理客户端的线程函数
void *handle_client(void *client_socket) {
    int sock = *(int *)client_socket;
    char buffer[BUFFER_SIZE];
    sqlite3 *db;

    // 打开数据库
    if (sqlite3_open("server.db", &db)) {
        printf("Can't open database: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    // 接收客户端数据
    int valread = read(sock, buffer, BUFFER_SIZE);  // 从客户端读取数据，存储到 buffer 中
    printf("Received from client: %s\n", buffer);

    // 将从客户端接收到的数据插入数据到数据库
    database_insert(db, buffer);

    // 发送确认信息给客户端，告知数据已接收并存储
    send(sock, "Data received and stored", strlen("Data received and stored"), 0);

    // 关闭数据库,关闭与客户端的套接字连接
    sqlite3_close(db);
    close(sock);
    free(client_socket);
    return NULL;
}

int main() {
    int server_fd, new_socket, *client_socket;
    struct sockaddr_in address;
    int opt = 1, addrlen = sizeof(address);

    // 创建 TCP 套接字,设置套接字选项，允许地址重用
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 设定服务器的 IP 地址和端口号
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 将套接字绑定到指定的 IP 地址和端口
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));

    // 使服务器监听传入的连接请求，允许最多 3 个待处理连接
    listen(server_fd, 3);

    // 接受客户端连接，并为每个连接分配一个新的套接字
    while (1) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        client_socket = malloc(sizeof(int));
        *client_socket = new_socket;

        // 创建新线程来处理客户端连接
        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, client_socket);
    }

    // 关闭服务器套接字
    close(server_fd);
    return 0;
}

