# 02-object-oriented-programming — Suites de Tests Python

Ce module regroupe les suites de validation et de tests unitaires pour la filière **Programmation Orientée Objet (Python)** du Common Core de 42.

---

## 📋 Modules Supportés

Chaque projet de la filière dispose d'une suite de validation de conformité de style (Flake8) et d'un harnais de tests unitaires :

| Module | Description & Objectifs | Runner associé |
| :--- | :--- | :--- |
| **`00-growing-code`** | Fondations Python : types, fonctions, boucles, formatting, structures de base (`ex0` à `ex7`) | `test-00-growing-code.sh` & `test-00-growing-code-main.sh` |
| **`01-code-cultivation`** | Modularité, packaging, gestion des exceptions et conventions | `test-01-code-cultivation.sh` |
| **`02-garden-guardian`** | Classes, encapsulation, héritage, polymorphisme | `test-02-garden-guardian.sh` |
| **`03-00-data-quest`** | Manipulation de données, parsing et structures adaptées | `test-03-00-data-quest.sh` |
| **`03-01-data-archivist`** | Persistance, sérialisation et flux de données | `test-03-01-data-archivist.sh` |
| **`03-02-nexus`** | Architecture réseau & communication inter-composants | `test-03-02-nexus.sh` |
| **`03-03-the-codex`** | Métaprogrammation et réflexivité | `test-03-03-the-codex.sh` |
| **`04-00-datadeck`** | Structures de données avancées | `test-04-00-datadeck.sh` |
| **`04-01-the-matrix`** | Calcul matriciel et modélisation orientée objet | `test-04-01-the-matrix.sh` |
| **`04-02-cosmic-data`** | Traitement & pipelines de données complexes | `test-04-02-cosmic-data.sh` |
| **`04-03-funcmage`** | Programmation fonctionnelle en Python (closures, générateurs, décorateurs) | `test-04-03-funcmage.sh` |

---

## 🚀 Outils de Validation

1. **Validation Flake8** :
   - Vérifie la conformité PEP 8 et l'absence d'erreurs de syntaxe ou variables non utilisées sur tous les fichiers du module (spécifiés dans `dist.files`).
2. **Suites `unittest` (`test_suite.py`)** :
   - Tests automatisés des classes, fonctions, valeurs limites et cas d'erreur.
3. **Contrôle statique des contraintes du sujet (AST)** :
   - Analyse syntaxique de l'arbre syntaxique abstrait (AST) pour faire respecter les règles strictes des sujets 42 :
     - **`03-01-data-archivist`** :
       - **Interdiction de `with` avant l'exercice 3** : vérifie par AST (`ast.With`, `ast.AsyncWith`) que ni `ex0`, `ex1`, ni `ex2` n'emploient de context managers prématurément.
       - **Types & collections standards autorisés** : vérifie l'absence d'imports de structures de collections non permises (`collections`, `heapq`, `array`, `queue`, `pandas`, `numpy`...) afin de restreindre l'usage aux seuls types autorisés (`str`, `int`, `float`, `list`, `dict`, `set`, `tuple`).
4. **Programme Helper Interactif & Automatisé (`main.py`)** :
   - Spécifiquement pour `00-growing-code`, un helper interactif permet d'exécuter et tester visuellement chaque exercice (`ex0` à `ex7`) ou de dérouler un scénario de test automatisé complet.

---

## 🛠️ Utilisation des Tests

### 1. Via les scripts de test

Les scripts détectent automatiquement le dossier cible (selon le dossier courant `$PWD` ou le chemin passé en argument).

#### Validation standard d'un module
```bash
# Tester 00-growing-code (Flake8 + unittest) :
./02-object-oriented-programming/test-00-growing-code.sh [/chemin/vers/00-growing-code]

# Tester n'importe quel autre module :
./02-object-oriented-programming/test-01-code-cultivation.sh [/chemin/vers/01-code-cultivation]
./02-object-oriented-programming/test-02-garden-guardian.sh [/chemin/vers/02-garden-guardian]
./02-object-oriented-programming/test-03-01-data-archivist.sh [/chemin/vers/03-01-data-archivist]
```

#### Options avancées pour `00-growing-code`
```bash
# Lancer les tests unitaires ET exécuter le runner main.py :
./02-object-oriented-programming/test-00-growing-code.sh --with-main

# Lancer directement le test helper main.py en mode automatique :
./02-object-oriented-programming/test-00-growing-code-main.sh [/chemin/vers/00-growing-code]

# Lancer main.py en mode interactif avec menu de sélection :
./02-object-oriented-programming/test-00-growing-code-main.sh --interactive

# Lancer un exercice spécifique (ex: ex4 ou ex7) :
./02-object-oriented-programming/test-00-growing-code-main.sh 4
```

---

### 2. Via `make`

Depuis la racine du dépôt `common-core-tests` :
```bash
# Tester un module Python spécifique :
make 00-growing-code
make 01-code-cultivation
make 02-garden-guardian

# Lancer le helper interactif de 00-growing-code :
make 00-growing-code-main

# Exécuter l'ensemble des suites de tests Python :
make python-tests
```
