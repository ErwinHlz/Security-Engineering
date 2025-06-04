#!/bin/bash

# Extract all table rows with teams
grep '<tr data-key=' fussball-tabelle.html > rows.txt

# Extract rank and team name from each row
sed -nE 's/.*ranking-number">([0-9]+)<\/td>.*wappen_[^>]*><\/span> ([^<]+)<\/a>.*/\1. \2/p' rows.txt > teams.txt

# Show the output
cat teams.txt

