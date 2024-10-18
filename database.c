#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

sqlite3 *db;
char *errMsg = NULL;
int rc;

// 处理SQLite查询的结果.
static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    int i;
    for (i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");//打印每一列的列名和对应的数据
    }
    printf("\n");
    return 0;
}

// Function to open database
int open_database() {
    rc = sqlite3_open("example.db", &db);//打开数据库
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return 0;
    } else {
        fprintf(stdout, "Opened database successfully\n");
    }
    return 1;
}

// Function to execute SQL query
int execute_sql(char* sql) {
    rc = sqlite3_exec(db, sql, callback, 0, &errMsg);//执行SQL语句
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return 0;
    }
    return 1;
}

// Function to insert data into database
void database_insert(char* info) {
    char sql[200];
    sprintf(sql, "INSERT INTO messages (info) VALUES ('%s');", info);//将数据格式化输出到一个字符串缓冲区sql
    if (!execute_sql(sql)) {
        fprintf(stderr, "Failed to insert data\n");
    }
}

// Function to select data from database
void database_select() {
    char *sql = "SELECT * from messages;";
    if (!execute_sql(sql)) {
        fprintf(stderr, "Failed to select data\n");
    }
}


// Initialize the database and create tables
void setup_database() {
    if (!open_database()) return;

    // Create SQL statement
    char *sql = "CREATE TABLE IF NOT EXISTS messages(" \
                "ID INTEGER PRIMARY KEY AUTOINCREMENT," \
                "info TEXT NOT NULL);";

    if (!execute_sql(sql)) {
        fprintf(stderr, "Failed to create table\n");
    }

    sqlite3_close(db);
}

int main(int argc, char* argv[]) {
    setup_database();
    open_database();

    // Example usage
    database_insert("Hello, World!");
    database_select();

    sqlite3_close(db);
    return 0;
}
