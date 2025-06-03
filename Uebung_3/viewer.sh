#!/bin/sh

if [ "$#" -eq 0 ]; then
	echo "Fehler: Bitte geben Sie nur eine Datei ein: "
	exit 1

elif [ "$#" -gt 1 ]; then
	echo "Fehler: Mehrere Dateien sind nicht erlaubt. Bitte geben Sie nur eine Datei ein:"
	exit 1
fi

filename="$1"

if [ ! -r "$filename" ]; then
	echo "Fehler: "$filename"  existiert nicht"
	exit 1
fi

filetype="$(file "$filename")"

if echo "$filetype" | grep -q "text"; then 
	echo "$filename ist eine Textdatei"
	less "$filename"

elif echo "$filetype" | grep -q "OpenDocument"; then
	echo "$filename ist eine OpenDocument-Datei"
	libreoffice "$filename"

elif echo "$filetype" | grep -q "executable"; then
	echo "$filename ist eine ausführbare Datei"
	sh "$filename"

elif echo "$filetype" | grep -q "image"; then
	echo "$filename ist eine Bilddatei"
	eog "$filename"

elif echo "$filetype" | grep -q "PDF document"; then
	echo "$filename ist eine PDF-Datei"
	xpdf "$filename"

else
	echo "$filename ist ein unbekannter Typ"
fi

exit 0
