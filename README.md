# 42 Common Core — Tests & Audits (`common-core-tests`)

Banc d'essai, suite de tests unitaires et plateforme d'audit mémoire pour les projets du **Common Core** de 42 (`libft`, `get_next_line`, `ft_printf`, `push_swap`).

Ce dépôt est totalement autonome et non-intrusif : il permet d'évaluer ses propres implémentations ou celles d'un pair en évaluation sans modifier le code source testé.

---

## 🚀 Fonctionnalités Clés

- **Isolation par sous-processus (`fork`)** : Les crashs (`SIGSEGV`, `SIGBUS`, etc.) sont interceptés sans interrompre le runner de test.
- **Audit mémoire in-process (`libft_mem_tracker`, `gnl_mem_tracker`)** : Interception au runtime (`-Wl,--wrap=malloc`, `calloc`, `free`) pour détecter au byte près les fuites, double-frees et frees invalides.
- **Injection de pannes `malloc` & mémoire sale (`0xAA`)** : Simulation systématique de heap non initialisé (`0xAA`) et d'échecs d'allocation mémoire pour vérifier que les fonctions retournent `NULL`, libèrent tous les blocs déjà alloués (0 leak) et ne crashent pas (`free(): invalid pointer`).
- **Recompilation automatique de la cible (`FORCE`)** : Toute modification apportée aux sources ou headers (`.c`, `.h`) du projet cible est automatiquement détectée et recompilée à la volée, sans re-link superflu si aucun fichier n'a changé.
- **Checker de référence intégré pour `push_swap`** : Simulation et validation pas à pas des piles `a` et `b`, statistiques et barème officiel 42 ($< 700$ pour 100, $< 5500$ pour 500).
- **Intégration Valgrind complète** : Détection des lectures/écritures invalides et vérification d'absence totale de fuites (`--leak-check=full`).
- **Scripts d'évaluation portables** : Un script par projet (`test_<projet>.sh`), exécutable depuis n'importe quel dossier ou sur un chemin cible, avec gestion transparente du cache de compilation (`CCACHE_DISABLE=1`).
- **Mode `.wip` informatif** : Si un marqueur `.wip` est présent dans le projet cible, les erreurs ne bloquent pas le build (utile en cours de développement).

---

## 📋 Prérequis par Projet (Norme 42)

Chaque projet a ses spécificités selon le sujet officiel 42 :

| Projet | `Makefile` requis ? | Fichiers sources attendus | Cible générée |
| :--- | :---: | :--- | :--- |
| **`libft`** | **Oui** (obligatoire) | Fichiers `ft_*.c`, `libft.h` | `libft.a` (règles `all`, `clean`, `fclean`, `re`) |
| **`ft_printf`** | **Oui** (obligatoire) | Sources printf, `ft_printf.h` | `libftprintf.a` (règles `all`, `clean`, `fclean`, `re`) |
| **`get_next_line`** | **Non** | `get_next_line.c`, `get_next_line_utils.c`, `get_next_line.h` | Aucune (compilation directe par le runner) |
| **`push_swap`** | **Oui** (obligatoire) | Sources `*.c`, `push_swap.h` | Exécutable `push_swap` (règles `all`, `clean`, `fclean`, `re`) |

---

## 📁 Architecture du Dépôt

```text
common-core-tests/
├── Makefile                 # Dispatcher global (libft, getnextline, libftprintf, push_swap, clean, fclean)
├── test_libft.sh            # Script d'évaluation autonome pour libft
├── test_getnextline.sh      # Script d'évaluation autonome pour get_next_line
├── test_libftprintf.sh      # Script d'évaluation autonome pour ft_printf
├── test_push_swap.sh        # Script d'évaluation autonome pour push_swap
├── 00-programming-fundamentals/
│   ├── 00-libft/            # Tests unitaires, fault injection malloc et wrappers mémoire
│   ├── 01-00-printf/        # Tests différentiels printf
│   ├── 01-01-getnextline/   # Tests automatiques, fault injection malloc et main interactif
│   └── 02-push-swap/        # Checker de référence, permutations aléatoires, benchmarks
└── 02-object-oriented-programming/ # Suites de tests unitaires Python (00-growing-code .. 04-03-funcmage)
```

---

## 🛠️ Utilisation

### 1. Via les scripts autonomes (Idéal Peer-Review & Tests rapides)

Les scripts acceptent soit aucun argument (teste le dossier courant `$PWD` ou cible par défaut), soit le chemin vers le projet à tester. L'option `--valgrind` (ou `-v`) active la vérification mémoire complète.

#### Push_swap
```bash
# Tester le dossier courant ou par défaut :
./test_push_swap.sh

# Tester un chemin spécifique (ex: clone d'un peer) :
./test_push_swap.sh /chemin/vers/push_swap

# Lancer avec Valgrind :
./test_push_swap.sh /chemin/vers/push_swap --valgrind
```

#### Libft
```bash
./test_libft.sh [/chemin/vers/libft] [--valgrind]
```

#### Get Next Line
```bash
./test_getnextline.sh [/chemin/vers/get_next_line] [--valgrind] [-b BUFFER_SIZE]
```

#### ft_printf
```bash
./test_libftprintf.sh [/chemin/vers/ft_printf] [--valgrind]
```

---

### 2. Via `make` directement

```bash
# Lancer les tests push_swap :
make push_swap PUSH_SWAP_DIR=/chemin/vers/push_swap
make -C 00-programming-fundamentals/02-push-swap valgrind PUSH_SWAP_DIR=/chemin/vers/push_swap

# Lancer tous les tests C :
make c-tests

# Lancer tous les tests Python :
make python-tests
```
