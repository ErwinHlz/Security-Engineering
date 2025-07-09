 #!/bin/bash
trap 'echo "→ File-Size-Limit überschritten."; exit 1' SIGXFSZ

        echo "====== File Size Limit Test ======"


 ulimit -f 1
        echo "File Size Limit gesetzt auf 1 Block = 512 Bytes."

        echo "→ Versuche, 10 KB in eine Datei zu schreiben..."
        cat ./hiiipower.txt > testfile
        echo "→ Datei wurde geschrieben (sollte eigentlich fehlschlagen)."

