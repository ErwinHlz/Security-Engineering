#!/bin/sh

if [ "$#" -ne 1 ]; then
	echo "Nur eine Datei darf als Argument übergegeben werden."
	exit 1
fi

filename="$1"

if [ ! -f "$filename" ] || echo "$filename" | grep -vq '\.wav$'; then
	echo "Fehler: "$filename" existiert nicht oder endet nicht mit .wav"
	exit 1
fi

mp3file="${1%.wav}.mp3"
ffmpeg -i "$1" "$mp3file"

if [ $? -ne 0 ]; then
	echo "Fehler bei der Umwandlung"
	exit 1
fi

exit 0

