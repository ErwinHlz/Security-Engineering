#!/bin/bash

trap 'echo "→ CPU-Zeit-Limit überschritten. Prozess wird beendet."; exit 99' SIGXCPU

ulimit -t 1  # CPU-Zeit auf 1 Sekunde begrenzen
echo "CPU-Zeit-Limit auf 1 Sekunde gesetzt."


# CPU-belastende Schleife
while true; do
    for i in {1..100000}; do
        :
    done
    sleep 0.1
done


