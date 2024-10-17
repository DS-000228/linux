#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

// 插入数据到数据库的函数
int database_insert(sqlite3 *db, const char *data) {
    char *err_msg = 0;
    char sql[256];

    // SQL 插入语句
    sprintf(sql, "INSERT INTO data_table(content) VALUES('%s');", data);

    // 执行插入语句
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to insert data: %s\n", err_msg);
        sqlite3_free(err_msg);
        return rc;
    }

    printf("Data inserted successfully\n");
    return SQLITE_OK;
}

// 从数据库查询所有数据并返回查询结果
int database_select(sqlite3 *db) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT * FROM data_table;";
    
    // 预编译 SQL 查询语句
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        printf("Failed to fetch data: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    // 循环读取查询结果的每一行
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);             // 获取 id 列数据
        const unsigned char *content = sqlite3_column_text(stmt, 1); // 获取 content 列数据
        printf("ID: %d, Content: %s\n", id, content);
    }

    // 释放查询语句句柄
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

// 打开数据库
sqlite3* open_database(const char *db_file) {
    sqlite3 *db;
    int rc = sqlite3_open(db_file, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    return db;
}

// 创建表格（如果不存在）
void create_table_if_not_exists(sqlite3 *db) {
    const char *sql = "CREATE TABLE IF NOT EXISTS data_table("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "content TEXT NOT NULL);";

    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        printf("Failed to create table: %s\n", err_msg);
        sqlite3_free(err_msg);
    } else {
        printf("Table created or already exists\n");
    }
}

// 关闭数据库
void close_database(sqlite3 *db) {
    sqlite3_close(db);
}

int main() {
    // 打开数据库
    sqlite3 *db = open_database("server.db");
    if (db == NULL) {
        return 1;
    }

    // 创建表格（如果不存在）
    create_table_if_not_exists(db);

    // 插入示例数据
    database_insert(db, "Example data from server");

    // 查询并打印数据库中的所有数据
    database_select(db);

    // 关闭数据库
    close_database(db);

    return 0;
}

