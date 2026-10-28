#!/bin/bash
# Pad a built ELF up to the exact target size (append-only, never truncate).
# A binary LARGER than the target is a hard budget violation and must fail.
set -euo pipefail
FILE="${1:?usage: pad_exact.sh <file> <target_bytes>}"
TARGET="${2:?usage: pad_exact.sh <file> <target_bytes>}"
size=$(stat -c '%s' "$FILE")
if [ "$size" -gt "$TARGET" ]; then
    echo "SIZE_BUDGET_VIOLATION: $FILE is $size bytes > target $TARGET bytes (payload too large — reduce, never truncate)" >&2
    exit 1
fi
pad=$((TARGET - size))
if [ "$pad" -gt 0 ]; then
    dd if=/dev/zero bs=65536 count=$((pad / 65536 + 1)) 2>/dev/null | head -c "$pad" >> "$FILE"
fi
final=$(stat -c '%s' "$FILE")
[ "$final" -eq "$TARGET" ] || { echo "PAD_FAILED: $final != $TARGET" >&2; exit 1; }
echo "PADDED: $FILE -> $final bytes (payload $size + pad $pad)"
