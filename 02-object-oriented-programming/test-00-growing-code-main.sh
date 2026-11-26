#!/usr/bin/env bash
# ==============================================================================
# Main.py Helper Runner for 02-object-oriented-programming / 00-growing-code
# Usage:
#   ./test-00-growing-code-main.sh [/path/to/00-growing-code] [--auto|-a] [--interactive|-i] [0-7|a]
# ==============================================================================
set -e

REAL_SCRIPT="$(readlink -f "${BASH_SOURCE[0]}")"
SCRIPT_DIR="$(cd "$(dirname "${REAL_SCRIPT}")" && pwd)"
SUBJECT="00-growing-code"
TARGET=""
MODE="--auto"
CHOICE=""

for arg in "$@"; do
    case "$arg" in
        --interactive|-i)
            MODE="--interactive"
            ;;
        --auto|-a)
            MODE="--auto"
            ;;
        [0-7]|a|all)
            CHOICE="$arg"
            ;;
        *)
            if [ -d "$arg" ]; then
                TARGET="$(cd "$arg" 2>/dev/null && pwd || echo "$arg")"
            fi
            ;;
    esac
done

if [ -z "$TARGET" ]; then
    if [ -f "${PWD}/dist.files" ]; then
        TARGET="${PWD}"
    elif [ -d "${SCRIPT_DIR}/../../object-oriented-programming/${SUBJECT}" ]; then
        TARGET="$(cd "${SCRIPT_DIR}/../../object-oriented-programming/${SUBJECT}" && pwd)"
    fi
fi

if [ -z "$TARGET" ] || [ ! -d "$TARGET" ]; then
    echo "❌ Erreur: Répertoire cible pour ${SUBJECT} introuvable."
    echo "Usage: $0 [/chemin/vers/${SUBJECT}] [--interactive|-i] [--auto|-a] [0-7|a]"
    exit 1
fi

MAIN_PY="${SCRIPT_DIR}/${SUBJECT}/main.py"
if [ ! -f "${MAIN_PY}" ]; then
    echo "❌ Erreur: ${MAIN_PY} introuvable."
    exit 1
fi

echo "================================================="
echo "  TEST HELPER RUNNER (main.py): ${SUBJECT}"
echo "  Target: ${TARGET}"
if [ "$MODE" = "--interactive" ]; then
    echo "  Mode:   Interactif"
else
    echo "  Mode:   Automatique (--auto)"
fi
echo "================================================="

export PYTHONPATH="${TARGET}:${PYTHONPATH}"

if [ "$MODE" = "--interactive" ]; then
    if [ -n "$CHOICE" ]; then
        python3 "${MAIN_PY}" "${TARGET}" "${CHOICE}"
    else
        python3 "${MAIN_PY}" "${TARGET}"
    fi
else
    # Automatic mode with mock inputs for non-interactive runs
    if [ -n "$CHOICE" ]; then
        python3 "${MAIN_PY}" "${TARGET}" "${CHOICE}" --auto
    else
        python3 "${MAIN_PY}" "${TARGET}" a --auto
    fi
fi

echo ""
echo "================================================="
echo "✅ EXÉCUTION DU PROGRAMME DE TEST (main.py) VALIDÉE !"
echo "================================================="
