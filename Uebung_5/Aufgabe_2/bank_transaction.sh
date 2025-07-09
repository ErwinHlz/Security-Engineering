#!/bin/bash

trap '' INT
# Trap INT verhindert Strg+C

echo "BANK TRANSACTION SYSTEM STARTED"

while true; do
    echo -n "Username: "
    read USERNAME

    if [ "$USERNAME" == "exit" ]; then
        echo "Programm terminiert."
        exit 0
    fi

    echo -n "Enter TAN: "
    read TAN_INPUT

    TAN_FILE="TAN/$USERNAME"

    if [ ! -f "$TAN_FILE" ]; then
        echo "keine TAN-Liste für $USERNAME gefunden."
        continue
    fi

    # Hole die aktuelle TAN (oberste Zeile)
    CURRENT_TAN=$(head -n 1 "$TAN_FILE")

    # Prüfe, ob Liste leer ist
    if [ -z "$CURRENT_TAN" ]; then
        echo "TAN-Liste ist leer. Bitte generiere neue TANs."
        continue
    fi

    # Prüfe Hash(TAN_input) == CURRENT_TAN
    HASHED_INPUT=$(echo -n "$TAN_INPUT" )


    if [ "$HASHED_INPUT" == "$CURRENT_TAN" ]; then
        echo "Zugriff erlaubt!"

        # TAN verbrauchen → erste Zeile entfernen
        tail -n +2 "$TAN_FILE" > "$TAN_FILE"



    else
        echo "Zugriff verweigert! Falsche TAN."

    fi

done
