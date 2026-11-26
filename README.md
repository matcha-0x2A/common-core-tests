# 42 Common Core — Tests & Audits (`common-core-tests`)

Plateforme de tests unitaires, audits mémoire et bancs d'essai automatisés pour les projets du **Common Core** de 42.

Ce dépôt est **autonome** et **non-intrusif** : il s'exécute contre les dépôts sources cibles sans altérer leur code ni imposer d'infrastructure lourde.

> [!IMPORTANT]
> ### ⚡ Nouveautés Majeures — Résilience aux Crashs & Filtre par Fonction
> - **Isolation Totale des Crashs** : Tout test qui segfault (par exemple `ft_strlcat(NULL, "Hello", 0)` si `dst == NULL` n'est pas géré) ou boucle indéfiniment est désormais isolé dans un sous-processus dédié avec mémoire partagée POSIX. **Le crash est consigné en échec et le déroulé des tests continue jusqu'au bout sans jamais s'arrêter.**
> - **Ciblage par Fonction / Groupe** : Vous pouvez exécuter les tests pour une seule fonction (`strlcat`, `ft_strlen`, etc.), plusieurs fonctions ou des groupes entiers (`part1`, `part2`, `bonus`) directement en ligne de commande ou via Make !

---

## 📚 Documentation par Module

L'ensemble des tests est structuré par module d'apprentissage du cursus 42 :

- 📘 [**00-programming-fundamentals/README.md**](file:///home/mchambon/wip/42/common-core-tests/00-programming-fundamentals/README.md)
  > Tests approfondis des projets C fondateurs :
  > - **`00-libft`** : tests unitaires, fault injection `malloc`, heap sale `0xAA`, traçage mémoire in-process (`-Wl,--wrap=malloc`).
  > - **`01-00-printf`** : tests différentiels exhaustifs vs `printf` standard libc.
  > - **`01-01-getnextline`** : gestion multi-fd, variations de `BUFFER_SIZE`, audit anti-leak.
  > - **`02-push-swap`** : conformité grille de soutenance v1.2 (14 questions), validation README, Norminette, Valgrind, benchmarks 100/500 ops.

- 🐧 [**01-systems-and-networks-administration/README.md**](file:///home/mchambon/wip/42/common-core-tests/01-systems-and-networks-administration/README.md)
  > Audit de conformité complet pour la virtualisation et l'administration système :
  > - **** : validation SSH, UFW, Sudo strict, PAM pwquality, LVM sur LUKS, monitoring et mapping de port VirtualBox.

- 🐧 [**01-systems-and-networks-administration/README.md**](file:///home/mchambon/wip/42/common-core-tests/01-systems-and-networks-administration/README.md)
  > Audit de conformité complet pour la virtualisation et l'administration système :
  > - **`00-born2beroot`** : validation SSH, UFW, Sudo strict, PAM pwquality, LVM sur LUKS, monitoring et mapping de port VirtualBox.

- 🐍 [**02-object-oriented-programming/README.md**](file:///home/mchambon/wip/42/common-core-tests/02-object-oriented-programming/README.md)
  > Suites de validation et tests unitaires pour la filière Python :
  > - **`00-growing-code`** à **`04-03-funcmage`** : validation de conformité Flake8 (PEP 8) et suites `unittest`.
  > - Programme de test helper (`main.py`) en mode automatique ou interactif pour `00-growing-code`.

---

## 📁 Architecture du Dépôt

```text
common-core-tests/
├── Makefile                          # Dispatcher global (libft, push_swap, python-tests, c-tests...)
├── README.md                         # Ce guide principal
│
├── 00-programming-fundamentals/      # Module C
│   ├── README.md                     # Documentation détaillée du module C
│   ├── test-00-libft.sh              # Runner autonome libft
│   ├── test-01-00-printf.sh          # Runner autonome ft_printf
│   ├── test-01-01-getnextline.sh     # Runner autonome get_next_line
│   ├── test-02-push-swap.sh          # Runner autonome push_swap
│   ├── 00-libft/                     # Harness libft, memory tracker & fault injection
│   ├── 01-00-printf/                 # Harness printf tests
│   ├── 01-01-getnextline/            # Harness get_next_line & main interactif
│   └── 02-push-swap/                 # Checker push_swap, benchmarks & audits
│
└── 02-object-oriented-programming/   # Module Python (OOP)
    ├── README.md                     # Documentation détaillée du module Python
    ├── test-00-growing-code.sh       # Runner Flake8 + unittest
    ├── test-00-growing-code-main.sh  # Runner pour le test helper main.py
    ├── 00-growing-code/              # test_suite.py & main.py
    └── ... (01-code-cultivation .. 04-03-funcmage)
```

---

## 🚀 Démarrage Rapide

### 1. Via `make` (depuis la racine)

```bash
# Afficher l'aide et les cibles disponibles
make help

# Lancer tous les tests C
make c-tests

# Lancer tous les tests Python
make python-tests

# Lancer un projet spécifique (avec cible explicite ou défaut)
make libft LIBFT_DIR=/chemin/vers/libft
make libft FUNCTION=strlcat                     # Tester une seule fonction
make push_swap PUSH_SWAP_DIR=/chemin/vers/push_swap
make 00-growing-code
```

### 2. Via les scripts de test

Chaque script peut être exécuté directement en lui passant (ou non) le chemin du projet :

```bash
# C - Projets fondamentaux
./00-programming-fundamentals/test-00-libft.sh [/chemin/vers/libft] [--valgrind] [fonction ...]
# Exemples libft :
./00-programming-fundamentals/test-00-libft.sh strlcat               # Fonction unique
./00-programming-fundamentals/test-00-libft.sh ft_strlcat ft_strlen # Plusieurs fonctions
./00-programming-fundamentals/test-00-libft.sh bonus                # Groupe (part1, part2, bonus)
./00-programming-fundamentals/test-01-00-printf.sh [/chemin/vers/ft_printf] [--valgrind]
./00-programming-fundamentals/test-01-01-getnextline.sh [/chemin/vers/get_next_line] [-b BUFFER_SIZE] [--valgrind]
./00-programming-fundamentals/test-02-push-swap.sh [/chemin/vers/push_swap] [--valgrind]

# Sysadmin - Administration Système & Réseau
./01-systems-and-networks-administration/test-00-born2beroot.sh [--auto] [-u <user>] [-p <pass>] [-r <root_pass>]
make born2beroot

# Python - Programmation orientée objet
./02-object-oriented-programming/test-00-growing-code.sh [/chemin/vers/00-growing-code]
./02-object-oriented-programming/test-00-growing-code-main.sh --interactive
```

---

## 🛡️ Principes de Fiabilité

- **Isolation par sous-processus** : Tous les tests unitaires C s'exécutent dans des sous-processus forkés ; aucun crash ne bloque le rapport complet.
- **Audit de mémoire stricte** : Détection des fuites, double-frees et accès hors limites via tracking in-process et Valgrind (`--leak-check=full`).
- **Tolérance au WIP (`.wip`)** : La présence d'un fichier `.wip` dans le dossier cible signale un travail en cours et exécute les tests à titre informatif sans bloquer les scripts.
