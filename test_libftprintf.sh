#!/usr/bin/env bash

# Usage:
#   ./test_libftprintf.sh               # tests current directory ($PWD) or libftprintf
#   ./test_libftprintf.sh /path/to/peer # tests specified directory
#   ./test_libftprintf.sh --valgrind    # tests with valgrind

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
    if [ "$PWD" != "$SCRIPT_DIR" ] && [ -f "${PWD}/Makefile" ]; then
        TARGET="${PWD}"
    elif [ -d "${ROOT_DIR}/libftprintf" ]; then
        TARGET="${ROOT_DIR}/libftprintf"
    fi
fi

if [ -z "$TARGET" ] || [ ! -d "$TARGET" ]; then
    echo "❌ Erreur: Répertoire cible introuvable."
    echo "Usage: $0 [/chemin/vers/ft_printf] [--valgrind]"
    exit 1
fi

if [ ! -f "$TARGET/Makefile" ]; then
    echo "❌ Erreur: Aucun Makefile trouvé dans '$TARGET'."
    echo "   ft_printf requiert un Makefile conforme aux consignes 42."
    exit 1
fi

TESTS_DIR="${SCRIPT_DIR}/libftprintf"

echo "=== Running ft_printf Tests against: $TARGET ==="
make -C "$TESTS_DIR" PRINTF_DIR="$TARGET" $VALGRIND_FLAG
