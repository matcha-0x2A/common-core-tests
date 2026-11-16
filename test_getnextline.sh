#!/usr/bin/env bash

# Usage:
#   ~/wip/x2a/tests/test_getnextline.sh               # tests current directory ($PWD) or getnextline
#   ~/wip/x2a/tests/test_getnextline.sh /path/to/peer # tests specified directory
#   ~/wip/x2a/tests/test_getnextline.sh --valgrind    # tests with valgrind

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

TARGET=""
VALGRIND_FLAG=""

for arg in "$@"; do
    if [ "$arg" = "--valgrind" ] || [ "$arg" = "-v" ]; then
        VALGRIND_FLAG="valgrind"
    else
        TARGET="$(cd "$arg" 2>/dev/null && pwd || echo "$arg")"
    fi
done

if [ -z "$TARGET" ]; then
    if [ -f "${PWD}/get_next_line.c" ]; then
        TARGET="${PWD}"
    else
        TARGET="${ROOT_DIR}/getnextline"
    fi
fi

TESTS_DIR="${SCRIPT_DIR}/getnextline"

echo "=== Running GetNextLine Tests against: $TARGET ==="
make -C "$TESTS_DIR" GNL_DIR="$TARGET" $VALGRIND_FLAG
