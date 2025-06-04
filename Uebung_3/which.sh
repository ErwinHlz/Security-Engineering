#!/bin/sh

if [ "$#" -lt 1 ]; then
    echo "Bitte mindestens ein Argument angeben"
    exit 1
fi

IFS=:

status=0

for kommando in "$@"; do
    gefunden=0
    for dir in $PATH; do
        if [ -x "$dir/$kommando" ]; then
            echo "$kommando gefunden in: $dir/$kommando"
            gefunden=1
            break
        fi
    done
    if [ "$gefunden" -eq 0 ]; then
        echo "$kommando wurde nicht im PATH gefunden"
        status=1
    fi
done

exit $status
