# 42 Common Core — Systems & Networks Administration Tests

Suite de tests automatisés et d'audit de conformité pour le module **Systems & Networks Administration** du Common Core 42.

## 📋 Projets Supportés

- **`00-born2beroot`** : Validation stricte des spécifications de la machine virtuelle :
  - **Vérification préliminaire en Phase 1** : comparaison du hash SHA1 du disque virtuel avec `signature.txt`
  - **Protection par Snapshot (activée par défaut)** : crée un snapshot de sécurité avant de lancer les tests pour figer le `.vdi` et restaurer l'état vierge à la fin
  - Détection automatique et ajout éventuel du port-forwarding VirtualBox (`4242`)
  - Contrôle d'accès interactif ou automatisé (comptes `root` et utilisateur standard)
  - Absence d'interface graphique (GUI) et conformité `multi-user.target`
  - Dimensionnement matériel (RAM 1024 Mo / Swap 1024 Mo)
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
- `--snapshot` : Active la sécurité par snapshot (créé avant le test pour ne pas altérer le disque de base, **activé par défaut**)
- `--no-snapshot` : Désactive la création du snapshot
- `--restore` : Restaure et supprime automatiquement le snapshot après les tests pour retrouver l'état vierge
- `--keep-snapshot` : Conserve le snapshot après les tests sans le supprimer
- `-u, --user <username>` : Nom de l'utilisateur standard (défaut : détecté dans `README.md` ou `mchambon`)
- `-p, --user-pass <password>` : Mot de passe de l'utilisateur standard
- `-r, --root-pass <password>` : Mot de passe du compte root
- `-P, --port <port>` : Port SSH de la machine hôte (défaut : `4242`)
- `-H, --host <host>` : Hôte SSH (défaut : `127.0.0.1`)
- `--vm <name>` : Nom de la machine VirtualBox (détection automatique)
- `--vdi <path>` : Chemin vers le fichier `.vdi` du disque virtuel
- `-y, --auto` : Mode non-interactif sans demande de confirmation
