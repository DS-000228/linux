#!/bin/bash

while true; do
    # 输入框让用户输入数据
    INPUT=$(dialog --inputbox "Enter data to send to server:" 8 40 3>&1 1>&2 2>&3 3>&-)
    
    # 检查是否取消
    if [ $? != 0 ]; then
        break
    fi

    # 调用客户端发送数据
    echo "$INPUT" | ./client

    # 查询数据库
    dialog --msgbox "Querying database..." 5 40
    sqlite3 server.db "SELECT * FROM data_table;" | dialog --textbox - 20 50

    # 继续选项
    dialog --yesno "Do you want to continue?" 6 25
    if [ $? != 0 ]; then
        break
    fi
done

