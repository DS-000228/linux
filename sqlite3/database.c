#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 定义数据库条目结构
typedef struct {
    int id;
    char name[50];
} DatabaseEntry;

// 函数声明
void database_insert(DatabaseEntry *db, int *size, int id, const char *name);
void database_select(DatabaseEntry *db, int size);

int main() {
    // 初始化数据库
    DatabaseEntry db[100]; // 假设最大100条数据
    int size = 0;

    // 插入数据
    database_insert(db, &size, 1, "Alice");
    database_insert(db, &size, 2, "Bob");
    
    // 选择并显示数据
    database_select(db, size);

    return 0;
}

// 插入数据到数据库
void database_insert(DatabaseEntry *db, int *size, int id, const char *name) {
    if (*size < 100) { // 检查数组大小
        db[*size].id = id;
        strncpy(db[*size].name, name, sizeof(db[*size].name) - 1);
        db[*size].name[sizeof(db[*size].name) - 1] = '\0'; // 确保字符串以null结束
        (*size)++;
    } else {
        printf("Database is full. Cannot insert new entry.\n");
    }
}

// 从数据库中选择并显示数据
void database_select(DatabaseEntry *db, int size) {
    printf("Database Entries:\n");
    for (int i = 0; i < size; i++) {
        printf("ID: %d, Name: %s\n", db[i].id, db[i].name);
    }
}

