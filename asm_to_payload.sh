#!/usr/bin/env bash

set -euo pipefail

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <file.asm>" >&2
    exit 1
fi

source_file="$1"

if [ ! -f "$source_file" ]; then
    echo "Error: file not found: $source_file" >&2
    exit 1
fi

output_file="$(mktemp)"
trap 'rm -f "$output_file"' EXIT

nasm -f bin "$source_file" -o "$output_file"

hexdump -C "$output_file" | awk '
BEGIN {
    bytes_per_line = 12
    count = 0
    printf "{\n    "
}

/^[[:xdigit:]]{8}/ {
    for (i = 2; i <= 17 && $i ~ /^[[:xdigit:]]{2}$/; i++) {
        printf "0x%s, ", $i
        count++

        if (count % bytes_per_line == 0)
            printf "\n    "
    }
}

END {
    print "\n};"
}
'