#!/usr/bin/env bash

# Usage:
#   ~/wip/x2a/tests/test_libft.sh               # tests current directory ($PWD)
#   ~/wip/x2a/tests/test_libft.sh /path/to/peer # tests specified directory
#   ~/wip/x2a/tests/test_libft.sh --valgrind     # tests with valgrind

TARGET="${PWD}"
VALGRIND_FLAG=""

for arg in "$@"; do
    if [ "$arg" = "--valgrind" ] || [ "$arg" = "-v" ]; then
        VALGRIND_FLAG="valgrind"
    else
        TARGET="$(cd "$arg" 2>/dev/null && pwd || echo "$arg")"
    fi
done

TESTS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/libft" && pwd)"

echo "=== Running Libft Tests against: $TARGET ==="
make -C "$TESTS_DIR" LIBFT_DIR="$TARGET" $VALGRIND_FLAG
