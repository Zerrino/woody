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

xxd -i "$output_file" | sed -n '/{/,/}/p' | sed '1s/.*{/ {/' | sed '$s/};/}/'
