#!/bin/sh

if [ "$#" -ne 1 ]; then
	echo "Bitte nur ein Argument angeben"
	exit 1
fi

kommando="$1"

IFS=:

for dir in $PATH; do
	if [ -x "$dir/$kommando" ]; then
		echo "$dir/$kommando"
		exit 0
	fi
done

exit 1
