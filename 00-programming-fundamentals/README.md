# 00-programming-fundamentals — Suites de Tests C

Ce module regroupe les suites de tests unitaires, audits mémoire et bancs d'essai pour les projets fondamentaux en langage C du Common Core de 42 : **`libft`**, **`ft_printf`**, **`get_next_line`** et **`push_swap`**.

---

## 📋 Projets Inclus & Prérequis

Chaque suite est conçue pour tester de manière autonome et non-intrusive le code d'un étudiant ou d'un pair en évaluation.

| Projet | Sujet & Cible | Attentes 42 | Outils & Vérifications |
| :--- | :--- | :--- | :--- |
| **`00-libft`** | Bibliothèque `libft.a` | Makefile conforme, fonctions standards & bonus (`ft_lst*.c`) | Audit mémoire in-process (`--wrap=malloc`), injection de pannes, heap `0xAA`, Valgrind |
| **`01-00-printf`** | Bibliothèque `libftprintf.a` | Makefile conforme, conversions `cspdiuxX%` | Tests différentiels comparés à la libc standard, gestion des pointeurs NULL et limites |
| **`01-01-getnextline`** | Sources directes | `get_next_line.c`, `get_next_line_utils.c`, `get_next_line.h` | Multi-fd, variations de `BUFFER_SIZE` (1, 42, 10M), leaks, crashs, injection d'erreurs |
| **`02-push-swap`** | Exécutable `push_swap` | Sujet v1.2, barèmes 100 et 500 nombres, conformité README, Norminette | Grille officielle 14 questions, benchmark 5 lignes `[bench]`, Valgrind, flags de stratégie |

---

## 🚀 Fonctionnalités Avancées

- **Isolation par sous-processus (`fork`)** : Les éventuels `SIGSEGV` ou `SIGBUS` sont capturés sans faire échouer l'ensemble du banc de test.
- **Audit mémoire in-process (`libft_mem_tracker`, `gnl_mem_tracker`)** : Grâce aux drapeaux de linkage `-Wl,--wrap=malloc`, `--wrap=free`, `--wrap=calloc`, chaque allocation est tracée afin de détecter fuites, double-frees et frees invalides.
- **Injection de fautes mémoire (`malloc failure`)** : Simulation de heap non initialisé (`0xAA`) et simulation d'échecs d'allocation pour tester la robustesse et l'absence de fuites sur retour `NULL`.
- **Mode `.wip` informatif** : Si le fichier `.wip` est présent à la racine du projet testé, les erreurs ne bloquent pas le build (pratique en phase de développement).

---

## 🛠️ Utilisation des Tests

### 1. Via les scripts de test

Chaque projet dispose d'un script bash dédié exécutable depuis n'importe où. Par défaut, le script cherche le projet dans les répertoires standards ou le dossier courant. Vous pouvez lui passer explicitement le chemin vers le projet à auditer.

#### `libft`
```bash
# Tester le répertoire par défaut ou courant :
./00-programming-fundamentals/test-00-libft.sh

# Tester un dossier spécifique (ex: clone d'un pair) :
./00-programming-fundamentals/test-00-libft.sh /chemin/vers/libft

# Activer l'audit Valgrind :
./00-programming-fundamentals/test-00-libft.sh /chemin/vers/libft --valgrind
```

#### `ft_printf`
```bash
./00-programming-fundamentals/test-01-00-printf.sh [/chemin/vers/ft_printf] [--valgrind]
```

#### `get_next_line`
```bash
# Tester avec BUFFER_SIZE par défaut (42) :
./00-programming-fundamentals/test-01-01-getnextline.sh [/chemin/vers/get_next_line]

# Tester avec un BUFFER_SIZE spécifique et Valgrind :
./00-programming-fundamentals/test-01-01-getnextline.sh /chemin/vers/get_next_line -b 1 --valgrind
```

#### `push_swap`
```bash
# Audit standard (grille des 14 questions officielles 42) :
./00-programming-fundamentals/test-02-push-swap.sh [/chemin/vers/push_swap]

# Audit approfondi avec Valgrind (64 tests complets) :
./00-programming-fundamentals/test-02-push-swap.sh /chemin/vers/push_swap --valgrind
```

---

### 2. Via `make`

Depuis la racine du dépôt `common-core-tests` :
```bash
# Lancer les tests d'un projet C particulier :
make 00-libft            # ou make libft
make 01-00-printf        # ou make libftprintf
make 01-01-getnextline   # ou make getnextline
make 02-push-swap        # ou make push_swap

# Spécifier le dossier cible :
make push_swap PUSH_SWAP_DIR=/chemin/vers/push_swap
make libft LIBFT_DIR=/chemin/vers/libft

# Lancer tous les tests C :
make c-tests
```

Depuis le sous-dossier de test :
```bash
cd 00-programming-fundamentals/00-libft && make
cd 00-programming-fundamentals/01-00-printf && make
cd 00-programming-fundamentals/01-01-getnextline && make
cd 00-programming-fundamentals/02-push-swap && make
```

---

## 📊 Grille d'Évaluation `push_swap` (14 Questions Officielles)

Le script de test `00-programming-fundamentals/test-02-push-swap.sh` intègre l'ensemble de la grille de soutenance :
1. **README Compliance** (première ligne italique, cursus 42, 4 complexités justifiées).
2. **Norminette** (`*.c` et `*.h`).
3. **Compilation & Makefile** (règles `all`, `clean`, `fclean`, `re`, zéro-relink).
4. **Memory Leaks** (audit Valgrind nominal, 100 nombres, erreurs, doublons).
5. **Gestion d'erreur** (`stderr`, code sortie non-zéro, format strict).
6. **Forçage des stratégies** (`--simple`, `--medium`, `--complex`, `--adaptive`).
7. **Identity Test** (0 opération sur listes déjà triées).
8. **Petites listes (3 & 5 nombres)** (seuils respectés $\le 3$ et $\le 12$ ops).
9. **Benchmark & Désordre** (validation du format 5 lignes `[bench]` sur `stderr`).
10. **Grandes listes (100 nombres)** ($< 700$ opérations).
11. **Comparatif 50 nombres** (affichage des 4 stratégies).
12. **Très grandes listes (500 nombres)** ($< 5500$ opérations).
13. **Live Coding** (détection et validation du flag de soutenance `--count-only`).
