#!/usr/bin/env bash

export CCACHE_DISABLE=1

# Usage:
#   ./test_push_swap.sh               # tests current directory ($PWD) or 02-push-swap
#   ./test_push_swap.sh /path/to/peer # tests specified directory
#   ./test_push_swap.sh --valgrind    # tests with valgrind

REAL_SCRIPT="$(readlink -f "${BASH_SOURCE[0]}")"
SCRIPT_DIR="$(cd "$(dirname "${REAL_SCRIPT}")" && pwd)"
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
    if [ "$PWD" != "$SCRIPT_DIR" ] && [ -f "${PWD}/Makefile" ] && [ ! -f "${PWD}/push_swap_tests.py" ]; then
        TARGET="${PWD}"
    elif [ -d "${SCRIPT_DIR}/../../programming-fundamentals/02-push-swap" ]; then
        TARGET="$(cd "${SCRIPT_DIR}/../../programming-fundamentals/02-push-swap" && pwd)"
    elif [ -d "${SCRIPT_DIR}/../../push-swap" ]; then
        TARGET="$(cd "${SCRIPT_DIR}/../../push-swap" && pwd)"
    elif [ -d "${SCRIPT_DIR}/../../push_swap" ]; then
        TARGET="$(cd "${SCRIPT_DIR}/../../push_swap" && pwd)"
    elif [ -d "${ROOT_DIR}/../programming-fundamentals/02-push-swap" ]; then
        TARGET="$(cd "${ROOT_DIR}/../programming-fundamentals/02-push-swap" && pwd)"
    elif [ -d "${ROOT_DIR}/02-push-swap" ] && [ ! -f "${ROOT_DIR}/02-push-swap/push_swap_tests.py" ]; then
        TARGET="${ROOT_DIR}/02-push-swap"
    fi
fi

if [ -z "$TARGET" ] || [ ! -d "$TARGET" ]; then
    echo "❌ Erreur: Répertoire cible introuvable."
    echo "Usage: $0 [/chemin/vers/push_swap] [--valgrind]"
    exit 1
fi

if [ ! -f "$TARGET/Makefile" ]; then
    echo "❌ Erreur: Aucun Makefile trouvé dans '$TARGET'."
    echo "   push_swap requiert un Makefile conforme aux consignes 42."
    exit 1
fi

TESTS_DIR="${SCRIPT_DIR}/02-push-swap"

echo "=== Running push_swap Tests against: $TARGET ==="
make -C "$TESTS_DIR" PUSH_SWAP_DIR="$TARGET" $VALGRIND_FLAG
