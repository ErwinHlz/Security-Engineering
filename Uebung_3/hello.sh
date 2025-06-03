#!/bin/sh

if [ "$#" -eq 0 ]; then 
        echo "Fehler: Bitte mindestens ein Argument eingeben"
        exit 1
fi      

for name in "$@"; do
        echo "Hallo $name"
done    

exit 0

