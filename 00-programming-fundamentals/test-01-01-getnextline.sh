#!/usr/bin/env bash

# Usage:
#   ./test_getnextline.sh               # tests current directory ($PWD) or getnextline
#   ./test_getnextline.sh /path/to/peer # tests specified directory
#   ./test_getnextline.sh --valgrind    # tests with valgrind
#   ./test_getnextline.sh -b 1          # tests with specific BUFFER_SIZE

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

TARGET=""
VALGRIND_FLAG=""
BUFFER_SIZE=""

while [ $# -gt 0 ]; do
    case "$1" in
        --valgrind|-v)
            VALGRIND_FLAG="valgrind"
            shift
            ;;
        -b|--buffer-size)
            BUFFER_SIZE="BUFFER_SIZE=$2"
            shift 2
            ;;
        *)
            TARGET="$(cd "$1" 2>/dev/null && pwd || echo "$1")"
            shift
            ;;
    esac
done

if [ -z "$TARGET" ]; then
    if [ -f "${PWD}/get_next_line.c" ]; then
        TARGET="${PWD}"
    elif [ -d "${ROOT_DIR}/getnextline" ]; then
        TARGET="${ROOT_DIR}/getnextline"
    fi
fi

if [ -z "$TARGET" ] || [ ! -d "$TARGET" ]; then
    echo "❌ Erreur: Répertoire cible introuvable."
    echo "Usage: $0 [/chemin/vers/get_next_line] [--valgrind] [-b BUFFER_SIZE]"
    exit 1
fi

if [ ! -f "$TARGET/get_next_line.c" ]; then
    echo "❌ Erreur: get_next_line.c introuvable dans '$TARGET'."
    echo "   (Rappel: get_next_line ne requiert pas de Makefile, mais nécessite get_next_line.c et get_next_line_utils.c)"
    exit 1
fi

TESTS_DIR="${SCRIPT_DIR}/01-01-getnextline"

echo "=== Running GetNextLine Tests against: $TARGET ==="
make -C "$TESTS_DIR" GNL_DIR="$TARGET" $VALGRIND_FLAG $BUFFER_SIZE
