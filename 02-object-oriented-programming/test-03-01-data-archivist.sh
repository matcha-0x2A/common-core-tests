#!/usr/bin/env bash
# ==============================================================================
# Runner for 02-object-oriented-programming / 03-01-data-archivist
# Usage:
#   ./test-03-01-data-archivist.sh [/path/to/03-01-data-archivist] [--with-main]
# ==============================================================================
set -e

REAL_SCRIPT="$(readlink -f "${BASH_SOURCE[0]}")"
SCRIPT_DIR="$(cd "$(dirname "${REAL_SCRIPT}")" && pwd)"
SUBJECT="03-01-data-archivist"
TARGET=""
WITH_MAIN=0

for arg in "$@"; do
    case "$arg" in
        --with-main|--all|-a)
            WITH_MAIN=1
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
    echo "Usage: $0 [/chemin/vers/${SUBJECT}] [--with-main]"
    exit 1
fi

echo "================================================="
echo "  TEST SUITE: ${SUBJECT}"
echo "  Target: ${TARGET}"
echo "================================================="

# 1. Vérification Flake8
echo ""
echo "--- 🔍 Étape 1 : Vérification Flake8 ---"
FLAKE8_BIN=$(command -v flake8 || echo "${HOME}/.local/bin/flake8")
if [ -x "${FLAKE8_BIN}" ] || command -v "${FLAKE8_BIN}" &>/dev/null; then
    "${FLAKE8_BIN}" --count --show-source --statistics "${TARGET}"
    echo "✅ Flake8: 100% Conforme (0 erreur)"
else
    echo "⚠️ Flake8 non trouvé, étape ignorée."
fi

# 2. Exécution des tests unitaires Pytest / Python
echo ""
echo "--- 🧪 Étape 2 : Tests unitaires Python ---"
TESTS_DIR="${SCRIPT_DIR}/${SUBJECT}"
export TARGET_DIR="${TARGET}"
export TESTS_DIR="${TESTS_DIR}"
export PYTHONDONTWRITEBYTECODE=1
export PYTHONPATH="${TARGET}:${PYTHONPATH}"

PYTEST_BIN=$(command -v pytest || echo "${HOME}/.local/bin/pytest")
if [ -d "${TESTS_DIR}" ]; then
    if [ -x "${PYTEST_BIN}" ] || command -v "${PYTEST_BIN}" &>/dev/null; then
        "${PYTEST_BIN}" -v "${TESTS_DIR}" --import-mode=importlib
    else
        python3 -m unittest discover -s "${TESTS_DIR}" -v -p "test_suite.py"
    fi
else
    echo "ℹ️ Aucune suite de test unitaire définie pour ${SUBJECT}."
fi

# 3. Exécution optionnelle du helper main.py
if [ "$WITH_MAIN" -eq 1 ]; then
    echo ""
    echo "--- 🚀 Étape 3 : Exécution du programme test (main.py) ---"
    "${SCRIPT_DIR}/test-03-01-data-archivist-main.sh" "${TARGET}" --auto
fi

echo ""
echo "================================================="
echo "✅ TOUS LES TESTS POUR ${SUBJECT} SONT VALIDÉS !"
echo "================================================="
