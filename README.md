# 42 Common Core — Tests & Audits (`common-core-tests`)

Banc d'essai, suite de tests unitaires et plateforme d'audit mémoire pour les projets du **Common Core** de 42 (`libft`, `get_next_line`, `ft_printf`).

Ce dépôt est totalement autonome et non-intrusif : il permet d'évaluer ses propres implémentations ou celles d'un pair en évaluation sans modifier le code source testé.

---

## 🚀 Fonctionnalités Clés

- **Isolation par sous-processus (`fork`)** : Les crashs (`SIGSEGV`, `SIGBUS`, etc.) sont interceptés sans interrompre le runner de test.
- **Audit mémoire in-process (`libft_mem_tracker`)** : Interception au runtime (`-Wl,--wrap=malloc`, `calloc`, `free`) pour détecter au byte près les fuites, double-frees et frees invalides.
- **Intégration Valgrind complète** : Détection des lectures/écritures invalides et vérification d'absence totale de fuites (`--leak-check=full`).
- **Scripts d'évaluation portables** : Un script par projet (`test_<projet>.sh`), exécutable depuis n'importe quel dossier ou sur un chemin cible.
- **Mode `.wip` informatif** : Si un marqueur `.wip` est présent dans le projet cible, les erreurs ne bloquent pas le build (utile en cours de développement).

---

## 📋 Prérequis par Projet (Norme 42)

Chaque projet a ses spécificités selon le sujet officiel 42 :

| Projet | `Makefile` requis ? | Fichiers sources attendus | Cible générée |
| :--- | :---: | :--- | :--- |
| **`libft`** | **Oui** (obligatoire) | Fichiers `ft_*.c`, `libft.h` | `libft.a` (règles `all`, `clean`, `fclean`, `re`) |
| **`ft_printf`** | **Oui** (obligatoire) | Sources printf, `ft_printf.h` | `libftprintf.a` (règles `all`, `clean`, `fclean`, `re`) |
| **`get_next_line`** | **Non** | `get_next_line.c`, `get_next_line_utils.c`, `get_next_line.h` | Aucune (compilation directe par le runner) |

> [!NOTE]
> Le sujet de `get_next_line` ne demande aucun Makefile. La suite de test compile donc directement les fichiers `.c` du dossier cible avec l'option `-D BUFFER_SIZE=...`. Pour `libft` et `ft_printf`, le test runner appelle le `Makefile` du projet avant d'injecter ses wrappers.

---

## 📁 Architecture du Dépôt

```text
common-core-tests/
├── Makefile                 # Dispatcher global (libft, getnextline, libftprintf, clean, fclean)
├── test_libft.sh            # Script d'évaluation autonome pour libft
├── test_getnextline.sh      # Script d'évaluation autonome pour get_next_line
├── test_libftprintf.sh      # Script d'évaluation autonome pour ft_printf
├── libft/                   # Suite de tests unitaires pour libft
│   ├── Makefile             # Compilation avec wrappers d'allocation (-Wl,--wrap=...)
│   ├── libft_mem_tracker.c  # Tracker d'allocation in-process
│   ├── libft_tests.c        # Runner et reporting
│   └── libft_*.c            # Suites de tests par fonction
├── getnextline/             # Suite de tests unitaires pour get_next_line
│   ├── Makefile
│   ├── gnl_tests.c          # Tests unitaires automatiques (stdin, multi-lignes, EOF...)
│   ├── gnl_tests.h
│   └── main.c               # Runner interactif (lecture stdin ou fichier avec délimiteurs colorés)
└── libftprintf/             # Suite de tests unitaires pour ft_printf
    ├── Makefile
    ├── printf_tests.c       # Tests comparatifs avec le printf système (%c, %s, %p, %d, ...)
    └── printf_tests.h
```

---

## 🛠️ Utilisation

### 1. Via les scripts autonomes (Idéal Peer-Review & Tests rapides)

Les scripts acceptent soit aucun argument (teste le dossier courant `$PWD`), soit le chemin vers le projet à tester. L'option `--valgrind` (ou `-v`) active la vérification mémoire complète.

#### Libft
```bash
# Tester le dossier courant :
./test_libft.sh

# Tester un chemin spécifique (ex: clone d'un peer) :
./test_libft.sh /chemin/vers/libft

# Lancer avec Valgrind :
./test_libft.sh /chemin/vers/libft --valgrind
```

#### Get Next Line
```bash
# Tester le dossier courant :
./test_getnextline.sh

# Tester un chemin spécifique :
./test_getnextline.sh /chemin/vers/get_next_line

# Lancer avec un buffer personnalisé :
./test_getnextline.sh /chemin/vers/get_next_line -b 1

# Lancer avec Valgrind :
./test_getnextline.sh /chemin/vers/get_next_line --valgrind

# Compiler et lancer le runner manuel/interactif (main.c avec coloration) :
make -C getnextline main GNL_DIR=/chemin/vers/get_next_line
./getnextline/gnl_main [fichier.txt]
# ou en lisant l'entrée standard (stdin) :
echo -e "line1\nline2" | ./getnextline/gnl_main
```

#### ft_printf
```bash
# Tester le dossier courant :
./test_libftprintf.sh

# Tester un chemin spécifique :
./test_libftprintf.sh /chemin/vers/ft_printf

# Lancer avec Valgrind :
./test_libftprintf.sh /chemin/vers/ft_printf --valgrind
```

---

### 2. Via `make` directement

Il est également possible de piloter les suites via `make` en spécifiant la variable de chemin du projet cible :

```bash
# --- Libft ---
make -C libft LIBFT_DIR=/chemin/vers/libft
make -C libft valgrind LIBFT_DIR=/chemin/vers/libft

# --- Get Next Line (taille de buffer configurable) ---
make -C getnextline GNL_DIR=/chemin/vers/gnl BUFFER_SIZE=42
make -C getnextline valgrind GNL_DIR=/chemin/vers/gnl BUFFER_SIZE=1

# --- ft_printf ---
make -C libftprintf PRINTF_DIR=/chemin/vers/ft_printf
make -C libftprintf valgrind PRINTF_DIR=/chemin/vers/ft_printf
```

Pour nettoyer les binaires de test générés :
```bash
make clean
# ou
make fclean
```
