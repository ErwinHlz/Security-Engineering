#!/bin/bash
ulimit -s 512
        echo "Stack Size Limit gesetzt auf 512 KB."

        # Funktion ruft sich endlos selbst auf:
        recursive() {
            recursive
        }
        recursive