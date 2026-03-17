#!/bin/bash -xe

# BROKER_IP="127.0.0.1"  # Replace with the IP address
# TOPIC="Vehicle_Control_Testing"

# echo "MQTT Publisher started. Type your messages (Ctrl+C to exit):"

# while true; do
#     read -p "Enter message (Trunk: Open/Trunk: Closed): " MESSAGE

#     if [ "$MESSAGE" != "Trunk: Open" ] && [ "$MESSAGE" != "Trunk: Closed" ]; then
#         echo "Invalid message. Only 'ON' and 'OFF' are allowed."
#     else
#         mosquitto_pub -h "$BROKER_IP" -p 1883 -t "$TOPIC" -m "$MESSAGE"
#         echo "Message sent: $MESSAGE"
#     fi
# done


BROKER_IP="192.168.79.189"  # Replace with the IP address
TOPIC="Vehicle_Control_Testing"

echo "MQTT Publisher started. Type your messages (Ctrl+C to exit):"

while true; do
    read -p "Enter message (Trunk: Open/Trunk: Closed): " MESSAGE

    #if [ "$MESSAGE" != "Trunk: Open" ] && [ "$MESSAGE" != "Trunk: Closed" ]; then
       # echo "Invalid message. Only 'ON' and 'OFF' are allowed."
    #else
        mosquitto_pub -h "$BROKER_IP" -p 1883 -t "$TOPIC" -m "$MESSAGE"
        echo "Message sent: $MESSAGE"
   # fi
done
