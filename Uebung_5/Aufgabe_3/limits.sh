#!/bin/bash

# Signal-Handler definieren
trap 'echo "→ CPU-Time Limit überschritten. Prozess wird beendet."; exit 1' SIGXCPU
trap 'echo "→ File-Size Limit überschritten. Prozess wird beendet."; exit 1' SIGXFSZ
trap 'echo "→ Segmentation Fault oder Stack Overflow. Prozess wird beendet."; exit 1' SIGSEGV

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
        ulimit -t 1
        echo "CPU-Time Limit gesetzt auf 1 Sekunde."
        # Endlosschleife, um CPU zu fressen
        while :; do :; done
        ;;
    2)
        echo ""
        echo "→ Stack-Limit-Test wird gestartet..."
        ulimit -s 512
        echo "Stack Size Limit gesetzt auf 512 KB."

        # Funktion ruft sich endlos selbst auf:
        recursive() {
            recursive
        }
        recursive
        ;;
    3)
        echo ""
        echo "→ File-Size-Limit-Test wird gestartet..."
        ulimit -f 1
        echo "File Size Limit gesetzt auf 1 Block = 512 Bytes."

        echo "→ Versuche, 10 KB in eine Datei zu schreiben..."
        dd if=/dev/zero of=testfile bs=1k count=10
        echo "→ Datei wurde geschrieben (sollte eigentlich fehlschlagen)."
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
