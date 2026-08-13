#!/usr/bin/env bash
# ============================================
# STUB ASSEMBLER & HEXDUMP CONVERTER
# Converts NASM assembly to C-compatible byte array format
# ============================================

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

# Produces pure machine code bytes for embedding in C
nasm -f bin "$source_file" -o "$output_file"

# CONVERT TO C-STYLE BYTE ARRAY
# xxd -i outputs: unsigned char stub[] = { 0x00, 0x01, ... };
xxd -i "$output_file" | sed -n '/{/,/}/p' | sed '1s/.*{/ {/' | sed '$s/};/}/'
