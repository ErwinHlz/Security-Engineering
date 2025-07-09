#!/bin/bash

# generate_tan <username> <number_of_tans>

if [ $# -ne 2 ]; then
    echo "Usage: $0 <username> <number_of_tans>"
    exit 1
fi

USERNAME="$1"
NUM="$2"

# Erstelle Verzeichnis für TANs
mkdir -p TAN

# zufälliger Startwert (Seed)
SEED=$(openssl rand -hex 16)

# tan_n = SEED
LAST_HASH="$SEED"

# Alle TANs in eine Liste hashen
TANS=()
for ((i=1; i<=NUM; i++)); do
    HASH=$(echo -n "$LAST_HASH" | sha256sum | awk '{print $1}')
    TANS+=("$HASH")
    LAST_HASH="$HASH"
done

# Die TAN-Liste in umgekehrter Reihenfolge speichern
# (d.h. TAN[1] ist die nächste zu verwendende TAN)
TAN_FILE="TAN/$USERNAME"

# überschreibe alte Datei
> "$TAN_FILE"

for ((i=NUM-1; i>=0; i--)); do
    echo "${TANS[$i]}" >> "$TAN_FILE"
done

echo "TAN Liste für $USERNAME mit $NUM TANs erstellt."
