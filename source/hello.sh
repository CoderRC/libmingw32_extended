#!/bin/sh

# Output file
OUT="output.txt"
: > "$OUT"   # clear the file first

# Find all files recursively (excluding output.txt itself)
find . -type f ! -name "$OUT" | sort | while read -r f; do
    echo "$f" >> "$OUT"
    cat "$f" >> "$OUT"
    echo "" >> "$OUT"
    echo "" >> "$OUT"
done
