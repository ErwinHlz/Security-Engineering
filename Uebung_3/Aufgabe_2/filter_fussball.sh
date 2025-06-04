#!/bin/bash

# th tags mit td tags ersetzen
sed -e 's:<th>:<td>:g' -e 's:</th>:</td>:g' fussball-tabelle.html > fussball-tabelle2.html

# Vergleich der dateien
echo "Unterschiede zwischen fussball-tabelle.html und fussball-tabelle2.html:"
diff -u fussball-tabelle.html fussball-tabelle2.html

# nach Bayern suchen. In welcher zeile liegt bayern
echo -e "\nZeilen, die 'Bayern München' enthalten:"
egrep -n 'Bayern' fussball-tabelle.html
echo -e "\n"

# formatierte liste erstellen mit fgrep und sed
fgrep '<tr data-key=' fussball-tabelle.html | sed -E 's/.*ranking-number">([0-9]+).* ([A-Z][a-zA-Z]*\.*[0-9]* [a-zA-Z0-9]*[A-Z][a-z]*|.*> [A-Z][a-z]*).*/\1. \2/g' | sed 's/mr10"><\/span> //g' > formatierteTabelle.txt

cat formatierteTabelle.txt
