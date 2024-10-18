#include <systemd/sd-bus.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

static int method_call(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open("example.db", &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    const char *text;
    // 读取字符串参数
    sd_bus_message_read(m, "s", &text);

    // SQL 插入命令
    char sql[256];
    snprintf(sql, sizeof(sql), "INSERT INTO Messages (Content) VALUES ('%s');", text);

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return -1;
    }

    sqlite3_close(db);
    return sd_bus_reply_method_return(m, "i", 0); // 成功
}

static const sd_bus_vtable add_message_vtable[]={
                                     SD_BUS_VTABLE_START(0),
                                     SD_BUS_METHOD("AddMessage", "s", "i", method_call, SD_BUS_VTABLE_UNPRIVILEGED),
                                     SD_BUS_VTABLE_END
                                 };

int main(void) {
    sd_bus_slot *slot = NULL;
    sd_bus *bus = NULL;
    int r;

    // 连接到系统总线
    r = sd_bus_open_user(&bus);
    if (r < 0) {
        fprintf(stderr, "Failed to connect to system bus: %s\n", strerror(-r));
        goto finish;
    }

    // 添加对象
    r = sd_bus_add_object_vtable(bus,
                                 &slot,
                                 "/net/poettering/dbus",   // 对象路径
                                 "net.poettering.dbus",    // 接口名
                                 add_message_vtable,
                                 NULL);
    if (r < 0) {
        fprintf(stderr, "Failed to issue method call: %s\n", strerror(-r));
        goto finish;
    }

    r = sd_bus_request_name(bus, "net.poettering.dbus", 0);
    if (r < 0) {
        fprintf(stderr, "Failed to acquire service name: %s\n", strerror(-r));
        goto finish;
    }

    // 获取请求
    for (;;) {
        r = sd_bus_process(bus, NULL);
        if (r < 0) {
            fprintf(stderr, "Failed to process bus: %s\n", strerror(-r));
            goto finish;
        }
        if (r > 0) /* we processed a request, try again immediately */
            continue;

        r = sd_bus_wait(bus, (uint64_t) -1);
        if (r < 0) {
            fprintf(stderr, "Failed to wait on bus: %s\n", strerror(-r));
            goto finish;
        }
    }
finish:
    sd_bus_slot_unref(slot);
    sd_bus_unref(bus);

    return 0;
}
