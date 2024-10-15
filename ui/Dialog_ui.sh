#!/bin/bash

while true; do
    cmd=$(dialog --menu "Select an action" 15 50 3 \
        1 "Insert Record" \
        2 "Query Database" \
        3 "Exit" 3>&1 1>&2 2>&3)

    case $cmd in
        1)
            no=$(dialog --inputbox "Enter No:" 8 40 3>&1 1>&2 2>&3)
            name=$(dialog --inputbox "Enter Name:" 8 40 3>&1 1>&2 2>&3)
            score=$(dialog --inputbox "Enter Score:" 8 40 3>&1 1>&2 2>&3)
            # Call the socket client here with $no, $name, $score
            ;;
        2)
            # Call the socket client to query the database
            ;;
        3)
            break
            ;;
    esac
done

