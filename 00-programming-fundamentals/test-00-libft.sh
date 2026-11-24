#!/usr/bin/env bash

export CCACHE_DISABLE=1

# Usage:
#   ./test_libft.sh               # tests current directory ($PWD)
#   ./test_libft.sh /path/to/peer # tests specified directory
#   ./test_libft.sh --valgrind    # tests with valgrind

REAL_SCRIPT="$(readlink -f "${BASH_SOURCE[0]}")"
SCRIPT_DIR="$(cd "$(dirname "${REAL_SCRIPT}")" && pwd)"
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
    elif [ -d "${SCRIPT_DIR}/../libft" ]; then
        TARGET="$(cd "${SCRIPT_DIR}/../libft" && pwd)"
    fi
fi

if [ -z "$TARGET" ] || [ ! -d "$TARGET" ]; then
    echo "❌ Erreur: Répertoire cible introuvable."
    echo "Usage: $0 [/chemin/vers/libft] [--valgrind]"
    exit 1
fi

if [ ! -f "$TARGET/Makefile" ]; then
    echo "❌ Erreur: Aucun Makefile trouvé dans '$TARGET'."
    echo "   Libft requiert un Makefile conforme aux consignes 42."
    exit 1
fi

TESTS_DIR="${SCRIPT_DIR}/00-libft"

echo "=== Running Libft Tests against: $TARGET ==="
make -C "$TESTS_DIR" LIBFT_DIR="$TARGET" $VALGRIND_FLAG
