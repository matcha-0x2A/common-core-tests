# 42 Common Core Workspace (`x2a`)

Espace de travail et banc d'ingénierie pour les projets du Common Core de 42.  
Ce dépôt sépare strictement le **code source livrable** (conforme aux exigences et au périmètre 42) de la **plateforme de test**, des **outils d'audit mémoire** et des **scripts de livraison**.

---

## 📁 Architecture du Dépôt

```text
├── tests/                       # Plateforme centrale de tests & audits
│   ├── Makefile                 # Dispatcher global des tests (make, make valgrind, ...)
│   ├── test_<projet>.sh         # Script d'évaluation portable (local ou peer-review)
│   └── <projet>/                # Suite de tests unitaire du module
│       ├── Makefile             # Compile le runner avec wrappers d'allocation
│       ├── libft_mem_tracker.c  # Tracker in-process (détection fuites au byte près)
│       ├── libft_tests.c        # Moteur d'exécution et reporting
│       └── libft_*.c            # Suites de tests par fonction
└── 
```


### 2. Validation & Tests unitaires avec audit mémoire
Les tests sont situés dans `tests/<projet>/` et utilisent une interception in-process des allocations (`-Wl,--wrap=malloc`, `calloc`, `free`) combinée à des exécutions isolées (`fork`) pour détecter les `SIGSEGV` sans crasher le runner :

```bash
# Lancer tous les tests unitaires du projet courant :
make -C tests

# Lancer la suite complète sous Valgrind (zéro leak, zéro invalid read/write) :
make -C tests valgrind

# Recompiler et relancer les tests d'un module spécifique :
make -C tests/libft re
```

### 3. Évaluation de pairs (Peer-Reviews)
Le script `test_<projet>.sh` permet d'évaluer le code d'un peer sans modifier son dépôt :
```bash
# Tester le dossier courant (ex: si tu es dans le clone du peer) :
~/wip/x2a/tests/test_libft.sh

# Tester un chemin spécifique :
~/wip/x2a/tests/test_libft.sh ~/reviews/un_peer

# Ajouter l'analyse Valgrind :
~/wip/x2a/tests/test_libft.sh ~/reviews/un_peer --valgrind
```


