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
FUNCTIONS=()

while [ $# -gt 0 ]; do
    case "$1" in
        --valgrind|-v)
            VALGRIND_FLAG="valgrind"
            shift
            ;;
        -f|--func|--function)
            FUNCTIONS+=("$2")
            shift 2
            ;;
        --help|-h)
            echo "Usage: $0 [/chemin/vers/libft] [--valgrind] [function_name ...]"
            exit 0
            ;;
        *)
            if [ -d "$1" ] && [ -z "$TARGET" ]; then
                TARGET="$(cd "$1" 2>/dev/null && pwd)"
            elif [ -z "$TARGET" ] && [[ "$1" == */* ]]; then
                TARGET="$1"
            else
                FUNCTIONS+=("$1")
            fi
            shift
            ;;
    esac
done

if [ -z "$TARGET" ]; then
    if [ "$PWD" != "$SCRIPT_DIR" ] && [ -f "${PWD}/Makefile" ]; then
        TARGET="${PWD}"
    elif [ -d "${SCRIPT_DIR}/../../programming-fundamentals/00-libft" ]; then
        TARGET="$(cd "${SCRIPT_DIR}/../../programming-fundamentals/00-libft" && pwd)"
    elif [ -d "${SCRIPT_DIR}/../../libft" ]; then
        TARGET="$(cd "${SCRIPT_DIR}/../../libft" && pwd)"
    fi
fi

if [ -z "$TARGET" ] || [ ! -d "$TARGET" ]; then
    echo "❌ Erreur: Répertoire cible introuvable."
    echo "Usage: $0 [/chemin/vers/libft] [--valgrind] [function_name ...]"
    exit 1
fi

if [ ! -f "$TARGET/Makefile" ]; then
    echo "❌ Erreur: Aucun Makefile trouvé dans '$TARGET'."
    echo "   Libft requiert un Makefile conforme aux consignes 42."
    exit 1
fi

TESTS_DIR="${SCRIPT_DIR}/00-libft"

if [ ${#FUNCTIONS[@]} -gt 0 ]; then
    echo "=== Running Libft Tests against: $TARGET (Filter: ${FUNCTIONS[*]}) ==="
    make -C "$TESTS_DIR" LIBFT_DIR="$TARGET" $VALGRIND_FLAG ARGS="${FUNCTIONS[*]}"
else
    echo "=== Running Libft Tests against: $TARGET ==="
    make -C "$TESTS_DIR" LIBFT_DIR="$TARGET" $VALGRIND_FLAG
fi
