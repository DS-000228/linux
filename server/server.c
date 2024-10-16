#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sqlite3.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// 数据库插入函数
void database_insert(sqlite3 *db, const char *data) {
    char *err_msg = 0;
    char sql[256];
    sprintf(sql, "INSERT INTO data_table(content) VALUES('%s');", data);
    sqlite3_exec(db, sql, 0, 0, &err_msg);
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
    int valread = read(sock, buffer, BUFFER_SIZE);
    printf("Received from client: %s\n", buffer);

    // 插入数据到数据库
    database_insert(db, buffer);

    // 发送确认信息给客户端
    send(sock, "Data received and stored", strlen("Data received and stored"), 0);

    // 关闭数据库
    sqlite3_close(db);
    close(sock);
    free(client_socket);
    return NULL;
}

int main() {
    int server_fd, new_socket, *client_socket;
    struct sockaddr_in address;
    int opt = 1, addrlen = sizeof(address);

    // 创建 TCP 套接字
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 绑定端口
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));

    // 监听连接
    listen(server_fd, 3);

    while (1) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        client_socket = malloc(sizeof(int));
        *client_socket = new_socket;

        // 创建一个线程来处理客户端连接
        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, client_socket);
    }

    close(server_fd);
    return 0;
}

