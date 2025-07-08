#!/bin/bash

# Wörterbuch-Datei (bitte anpassen, falls du die Datei an anderer Stelle abgelegt hast)
WORDLIST="words.txt"

# Nutzerliste im Format:
# Benutzername:Salt:Hash
USERS="
Steffi.Jones:O7v0C21Z:2FH7ib2Dxtoq6B83qTgON1
Marco.Reus:Jebn3vQ5:2k..iqxtXNwfsCFAamWCS0
Almuth.Schult:0ngrMRa1:uXLzWhnrYzmiRM3fi8Nde1
Manuel.Neuer:1aaPttrp:VoF2rkOyC/tE.DxzQuuIY1
Birgit.Prinz:7ieEwjFr:T/jwatbzqhLZNVDEfymB41
"


# Für jeden User durchlaufen
while IFS= read -r user_entry; do
    # Überspringe leere Zeilen
    [ -z "$user_entry" ] && continue

    USERNAME=$(echo "$user_entry" | cut -d':' -f1)
    SALT=$(echo "$user_entry" | cut -d':' -f2)
    TARGET_HASH=$(echo "$user_entry" | cut -d':' -f3)

    echo "Cracking password for $USERNAME ..."
    echo "Username: $USERNAME"
    echo "Salt: $SALT"
    echo "Target Hash: $TARGET_HASH"


    FOUND=0

    # Durchlaufe alle Wörter im Wörterbuch
    while IFS= read -r word; do
        # Hash generieren
        HASH=$(openssl passwd -1 -salt "$SALT" "$word" | cut -d'$' -f4)

        if [ "$HASH" == "$TARGET_HASH" ]; then
            echo "[+] Password found for $USERNAME: $word"
            FOUND=1
            break
        fi
    done < "$WORDLIST"

    if [ $FOUND -eq 0 ]; then
        echo "[-] No password found for $USERNAME"
    fi

done <<< "$USERS"
