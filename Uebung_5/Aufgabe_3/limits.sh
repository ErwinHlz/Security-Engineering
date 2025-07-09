#!/bin/bash

# Signal-Handler definieren
trap 'echo "→ CPU-Time Limit überschritten. Prozess wird beendet."; exit 1' SIGXCPU
trap 'echo "→ File-Size Limit überschritten. Prozess wird beendet."; exit 1' SIGXFSZ
trap 'echo "→ Segmentation Fault oder Stack Overflow. Prozess wird beendet."; exit 1' 11

# traps funktionieren hier nicht, da der kernel immer greift, um das programm zu beenden und
# deshalb kann er zu dem zeitpunkt keinen code mehr ausführen.

echo "====== Limit-Test-Programm ======"

echo "Was möchtest du testen?"
echo "1) CPU-Limit überschreiten"
echo "2) Stack-Limit überschreiten"
echo "3) File-Size-Limit überschreiten"
echo "0) Beenden"

read -p "Bitte Zahl eingeben: " WAHL

case "$WAHL" in
    1)
        echo ""
        echo "→ CPU-Limit-Test wird gestartet..."

        # Endlosschleife, um CPU zu fressen
        ./cpu_kill.sh
        EXITCODE=$?


                if [ $EXITCODE -eq 152 ]; then
                  echo "→ CPU-Zeit-Limit überschritten! (SIGXCPU)"
                fi
                if [ $EXITCODE -eq 137 ]; then
                  echo "Programm wurde durch SIGKILL beendet."
                fi


        ;;
    2)
        echo ""
        echo "→ Stack-Limit-Test wird gestartet..."

        # Funktion ruft sich endlos selbst auf:
        ./stack_kill.sh
        EXITCODE=$?

        if [ $EXITCODE -eq 139 ]; then
          echo "→ Stack Overflow erkannt! (SIGSEGV)"
        fi


        # Funktion ruft sich endlos selbst auf:

        ;;
    3)
        echo ""
        echo "→ File-Size-Limit-Test wird gestartet..."
        trap 'echo ">>> Datei zu groß (SIGXFSZ)!"' SIGXFSZ

        ulimit -f 1

        # Bash-intern schreiben:
        cat ./hiiipower.txt > testfile
        EXITCODE=$?

        echo "$EXITCODE"

        if [ $EXITCODE -eq 153 ]; then


            echo "Dateigrößen-Limit überschritten (SIGXFSZ) erkannt!"

        fi


        ;;
    0)
        echo "→ Programm beendet."
        exit 0
        ;;
    *)
        echo "Ungültige Auswahl."
        ;;
esac

echo "Programm fertig."

# in Bash simuliert:
# 418  trap 'echo "SIGXFSZ ausgelöst, datei limit überschritten"' SIGXFSZ
#  419  kill -SIGXFSZ $$  -> SIGXFSZ ausgelöst, datei limit überschritten
#  420  ulimit -f 1
#  421  cat hiiipower.txt > testfile.txt -> File size limit exceeded (core dumped)
#  422  kill -SIGXFSZ $$ -> SIGXFSZ ausgelöst, datei limit überschritten
#  423  history
# äquivalent zu andren Befehlen, da kernel immer greift, um das Programm zu beenden
