#!/bin/bash

# Parametri
PORT=8080
ADDRESS="127.0.0.1"
FILE="input.txt"
PC=7
PS=2
KEY="IamaKey!"

OUTPUT="server_output.txt"
MAX_CONN=2


# Esecuzione del server
echo "Avvio del server in corso..."
./server.exe -d "$PORT" -p "$PS" -f "$OUTPUT" -i "$MAX_CONN" &

sleep 2

# Esecuzione del client
echo "Avvio del/dei client 1 in corso..."
./client.exe -d "$PORT" -a "$ADDRESS" -f "$FILE" -p "$PC" -k "$KEY" &

