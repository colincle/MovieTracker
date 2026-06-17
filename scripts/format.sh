#!/bin/bash

command -v clang-format >/dev/null 2>&1 || { echo "clang-format is not installed. Aborting."; exit 1; }

DIR=${1:-.}

mkdir -p "$DIR/orig"

while IFS= read -r -d '' file; do
    cp "$file" "$DIR/orig/$(basename "$file").orig"
    echo "Formatting $file"
    clang-format -i "$file"
done < <(find "$DIR" -type f \( -iname "*.cpp" -o -iname "*.hpp" \) -print0)

echo "Formatting completed. Backups saved to $DIR/orig/"
