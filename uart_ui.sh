#!/bin/bash

#DB_FILE="example.db"
send_data(){   
    echo "$1" | nc localhost 9999  # Assuming the socket server is running on localhost and port 9999
}
send_data_to_server() {
    DATA=$(dialog --inputbox "Enter data to send to server:" 8 40 3>&1 1>&2 2>&3)
    RESPONSE=$(send_data "$DATA")
    if [ $? -ne 0 ]; then
        return
    fi
    continue
#    dialog --msgbox "Server response: $RESPONSE" 8 40
}

# Function to query database and display results
display_data() {
    dialog --title "Database Query Results" --msgbox "$1" 10 50
}
query_database() {
    display_data "$RESPONSE"
#    QUERY_RESULT=$(sqlite3 $DB_FILE "SELECT * FROM info;")
#    dialog --msgbox "$QUERY_RESULT" 15 40
}

while true; do
    CHOICE=$(dialog --menu "Choose an option" 15 40 3 \
        1 "Send data to server" \
        2 "Query database" \
        3 "Exit" 3>&1 1>&2 2>&3)
    
    if [ $? -ne 0 ]; then
        break
    fi

    case $CHOICE in
        1)
            send_data_to_server
            if [ $? -ne 0 ]; then
                continue
            fi
            ;;
        2)
            query_database
            ;;
        3)
            break
            ;;
    esac
done

clear
