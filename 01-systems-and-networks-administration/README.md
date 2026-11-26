# 42 Common Core — Systems & Networks Administration Tests

Suite de tests automatisés et d'audit de conformité pour le module **Systems & Networks Administration** du Common Core 42.

> [!NOTE]
> **Tested for Debian only** : Cette suite de tests d'audit a été développée et validée spécifiquement pour **Debian**. Les règles UFW, PAM et paquets système sont ciblés pour l'environnement Debian.

## 📋 Projets Supportés

- **`00-born2beroot`** : Validation stricte des spécifications de la machine virtuelle :
  - **Vérification préliminaire en Phase 1** : comparaison du hash SHA1 du disque virtuel avec `signature.txt`
  - **Protection par Snapshot (activée par défaut)** : crée un snapshot de sécurité avant de lancer les tests pour figer le `.vdi` et restaurer l'état vierge à la fin
  - Détection automatique et ajout éventuel du port-forwarding VirtualBox (`4242`)
  - Contrôle d'accès interactif ou automatisé (comptes `root` et utilisateur standard)
  - Absence d'interface graphique (GUI) et conformité `multi-user.target`
  - Dimensionnement matériel (Swap en adéquation : SWAP >= RAM)
  - Partitionnement sécurisé (LVM sur conteneur chiffré LUKS, `/boot` en clair)
  - Politique de sécurité sudo (tentatives, journalisation E/S dans `/var/log/sudo/`, requiretty, secure_path)
  - Pare-feu UFW (règles par défaut, port 4242 seul autorisé)
  - OpenSSH (port non-standard 4242, `PermitRootLogin no`, blocage effectif de root)
  - Politiques d'expiration et de complexité des mots de passe (`login.defs`, `pam_pwquality`)
  - Script de télémétrie (`monitoring.sh`) et planification cron

---

## 🚀 Utilisation

### Via Make (depuis la racine du dépôt)
```bash
# Lancer les tests Born2beRoot (mode interactif ou paramètres détectés)
make born2beroot

# Spécifier le dossier du projet ou des options
make born2beroot ARGS="--auto"
make born2beroot ARGS="--restore"
```

### Directement via le script
```bash
./01-systems-and-networks-administration/test-00-born2beroot.sh [options] [/chemin/vers/00-born2beroot]
```

#### Options principales :
- `-c, --config <file>` : Chemin vers un fichier de configuration `test.config` personnalisé
- `--snapshot` : Active la sécurité par snapshot (créé avant le test pour ne pas altérer le disque de base, **activé par défaut**)
- `--no-snapshot` : Désactive la création du snapshot
- `--restore` : Restaure et supprime automatiquement le snapshot après les tests pour retrouver l'état vierge
- `--keep-snapshot` : Conserve le snapshot après les tests sans le supprimer
- `-u, --user <username>` : Nom de l'utilisateur standard (défaut : `$USER`)
- `-p, --user-pass <password>` : Mot de passe de l'utilisateur standard
- `-r, --root-pass <password>` : Mot de passe du compte root
- `-P, --port <port>` : Port SSH de la machine hôte (défaut : `4242`)
- `-H, --host <host>` : Hôte SSH (défaut : `127.0.0.1`)
- `--vm <name>` : Nom de la machine VirtualBox (détection automatique)
- `--vdi <path>` : Chemin vers le fichier `.vdi` du disque virtuel
- `-y, --auto` : Mode non-interactif sans demande de confirmation

---

## ⚙️ Configuration Automatique (`test.config`)

Pour éviter d'avoir à saisir vos identifiants à chaque exécution du test, un fichier de configuration `test.config` peut être placé **directement dans le dossier testé** (le répertoire de votre projet contenant `signature.txt`) :
- Dans le dossier de votre projet testé : `/chemin/vers/mon_projet/test.config`
- Ou dans le répertoire courant si vous lancez les commandes depuis votre projet : `./test.config`
- Ou spécifié manuellement via l'option : `-c /chemin/vers/test.config`.

### Mise en place depuis le template par défaut (`test.config.default`) :

```bash
# Copier le template dans le dossier testé de votre projet
cp 01-systems-and-networks-administration/test.config.default /chemin/vers/mon_projet/test.config
```

### Variables supportées :

| Variable | Description | Valeur par défaut |
| :--- | :--- | :--- |
| `B2BR_USER` | Identifiant utilisateur standard | `$USER` (si non renseigné) |
| `B2BR_USER_PASS` | Mot de passe utilisateur standard | *(requis pour le mode automatique)* |
| `B2BR_ROOT_PASS` | Mot de passe du compte root | *(requis pour le mode automatique)* |
| `B2BR_HOST` | Hôte / adresse IP pour SSH | `127.0.0.1` |
| `B2BR_PORT` | Port SSH de redirection NAT | `4242` |
| `VM_NAME` | Nom de la machine VirtualBox | *(détecté automatiquement)* |
| `VDI_PATH` | Chemin vers le disque virtuel `.vdi` | *(détecté automatiquement)* |

> ℹ **Comportement si non paramétré :**
> Si aucun fichier `test.config` n'est détecté dans le dossier testé ou s'il n'est pas complètement paramétré (mots de passe non renseignés), le script affiche en une ligne une astuce rappelant son existence :
> ```text
> ℹ Astuce : Vous pouvez placer un fichier 'test.config' dans le dossier testé (voir test.config.default) pour renseigner automatiquement vos identifiants.
> ```
> puis pose interactivement les questions pour demander l'utilisateur et les mots de passe.

