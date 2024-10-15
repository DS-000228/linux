#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

void database_insert(sqlite3 *db, int no, const char *name, float score) {
    char *errmsg;
    char sqlstr[128];
    sprintf(sqlstr, "INSERT INTO student (no, name, score) VALUES (%d, '%s', %.1f);", no, name, score);
    
    if (sqlite3_exec(db, sqlstr, NULL, NULL, &errmsg) != SQLITE_OK) {
        printf("Insert error: %s\n", errmsg);
        sqlite3_free(errmsg);
    } else {
        printf("Insert successful\n");
    }
}

void database_select(sqlite3 *db) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT * FROM student;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        printf("Failed to fetch data: %s\n", sqlite3_errmsg(db));
        return;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("No: %d, Name: %s, Score: %.1f\n",
               sqlite3_column_int(stmt, 0),
               sqlite3_column_text(stmt, 1),
               sqlite3_column_double(stmt, 2));
    }
    sqlite3_finalize(stmt);
}

