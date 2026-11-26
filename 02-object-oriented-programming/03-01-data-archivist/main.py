#!/usr/bin/env python3

"""
Helper test runner for 03-01-data-archivist.

Usage:
    python3 main.py
    python3 main.py --auto
    python3 main.py /path/to/03-01-data-archivist
    python3 main.py /path/to/target 0 --auto
"""

import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile


sys.dont_write_bytecode = True

TESTS_DIR = Path(__file__).resolve().parent
TEST_FRAGMENT_FILE = TESTS_DIR / "ancient_fragment.txt"


def find_target_directory(given_path=None) -> Path:
    """Localise le dossier cible contenant les exercices du sujet."""
    candidates = []
    if given_path:
        candidates.append(Path(given_path).resolve())
    if "TARGET" in os.environ:
        candidates.append(Path(os.environ["TARGET"]).resolve())
    if "TARGET_DIR" in os.environ:
        candidates.append(Path(os.environ["TARGET_DIR"]).resolve())
    if "DATA_ARCHIVIST_DIR" in os.environ:
        candidates.append(Path(os.environ["DATA_ARCHIVIST_DIR"]).resolve())

    cwd = Path.cwd().resolve()
    candidates.append(cwd)

    script_dir = TESTS_DIR
    candidates.append(script_dir)

    for cand in candidates:
        if cand.is_dir() and (
            (cand / "ex0").exists() or (cand / "dist.files").exists()
        ):
            return cand

    return cwd if (cwd / "ex0").exists() else script_dir


def find_script(
    target_dir: Path, exercise_name: str, script_name: str
) -> Path:
    """Localise un script pour un exercice donné."""
    exact = target_dir / exercise_name / script_name
    if exact.is_file():
        return exact
    sub = target_dir / exercise_name
    if sub.is_dir():
        for item in sub.iterdir():
            if item.suffix == ".py" and not item.name.startswith("."):
                return item
    return exact


def test_ex0(base_path: Path) -> bool:
    """Teste ex0: ft_ancient_text."""
    print("\n" + "=" * 50)
    print("  🧪 Test de ex0/ft_ancient_text.py")
    print("=" * 50)

    script_path = find_script(base_path, "ex0", "ft_ancient_text.py")
    if not script_path.exists():
        print(f"❌ Script manquant: {script_path}")
        return False

    temp_dir = Path(tempfile.mkdtemp(prefix="test_ex0_"))
    try:
        print(
            f"→ Test 1: Lecture d'un fichier existant "
            f"({TEST_FRAGMENT_FILE.name}) :"
        )
        p = subprocess.run(
            [sys.executable, str(script_path), str(TEST_FRAGMENT_FILE)],
            capture_output=True,
            text=True,
            cwd=temp_dir,
        )
        print(p.stdout)
        if p.stderr:
            print("[STDERR]:", p.stderr)

        if "Cyber Archives" not in p.stdout or "FRAGMENT 001" not in p.stdout:
            print("❌ Échec: Sortie attendue non détectée sur fichier valide.")
            return False

        print(
            "→ Test 2: Lecture d'un fichier inexistant "
            "(unknown_vault.txt) :"
        )
        missing_file = temp_dir / "unknown_vault.txt"
        p_missing = subprocess.run(
            [sys.executable, str(script_path), str(missing_file)],
            capture_output=True,
            text=True,
            cwd=temp_dir,
        )
        print(p_missing.stdout)
        if p_missing.stderr:
            print("[STDERR]:", p_missing.stderr)

        comb = (p_missing.stdout + p_missing.stderr).lower()
        if not any(k in comb for k in ("error", "not found", "errno")):
            print("❌ Échec: Erreur non gérée pour fichier inexistant.")
            return False

        print("✅ ex0 validé avec succès !")
        return True
    finally:
        shutil.rmtree(temp_dir, ignore_errors=True)


def test_ex1(base_path: Path) -> bool:
    """Teste ex1: ft_archive_creation."""
    print("\n" + "=" * 50)
    print("  🧪 Test de ex1/ft_archive_creation.py")
    print("=" * 50)

    script_path = find_script(base_path, "ex1", "ft_archive_creation.py")
    if not script_path.exists():
        print(f"❌ Script manquant: {script_path}")
        return False

    temp_dir = Path(tempfile.mkdtemp(prefix="test_ex1_"))
    try:
        output_file = "saved_archive.txt"
        print(f"→ Test 1: Transformation et sauvegarde dans '{output_file}' :")
        p = subprocess.run(
            [sys.executable, str(script_path), str(TEST_FRAGMENT_FILE)],
            input=f"{output_file}\n",
            capture_output=True,
            text=True,
            cwd=temp_dir,
        )
        print(p.stdout)
        if p.stderr:
            print("[STDERR]:", p.stderr)

        saved = temp_dir / output_file
        if not saved.is_file():
            print(f"❌ Échec: Le fichier '{output_file}' n'a pas été créé.")
            return False

        print("→ Test 2: Annulation de sauvegarde (nom vide) :")
        p_cancel = subprocess.run(
            [sys.executable, str(script_path), str(TEST_FRAGMENT_FILE)],
            input="\n",
            capture_output=True,
            text=True,
            cwd=temp_dir,
        )
        print(p_cancel.stdout)

        print("✅ ex1 validé avec succès !")
        return True
    finally:
        shutil.rmtree(temp_dir, ignore_errors=True)


def test_ex2(base_path: Path) -> bool:
    """Teste ex2: ft_stream_management."""
    print("\n" + "=" * 50)
    print("  🧪 Test de ex2/ft_stream_management.py")
    print("=" * 50)

    script_path = find_script(base_path, "ex2", "ft_stream_management.py")
    if not script_path.exists():
        print(f"❌ Script manquant: {script_path}")
        return False

    temp_dir = Path(tempfile.mkdtemp(prefix="test_ex2_"))
    try:
        print("→ Test 1: Sortie normale sur stdout (sans erreur stderr) :")
        p = subprocess.run(
            [sys.executable, str(script_path), str(TEST_FRAGMENT_FILE)],
            input="\n",
            capture_output=True,
            text=True,
            cwd=temp_dir,
        )
        print(p.stdout)
        if p.stderr:
            print(f"❌ Échec: Présence anormale de stderr: {p.stderr}")
            return False

        print("→ Test 2: Redirection d'erreur sur stderr ([STDERR]) :")
        missing_corrupt = temp_dir / "corrupted.txt"
        p_err = subprocess.run(
            [sys.executable, str(script_path), str(missing_corrupt)],
            input="\n",
            capture_output=True,
            text=True,
            cwd=temp_dir,
        )
        print("[STDOUT]:", p_err.stdout)
        print("[STDERR]:", p_err.stderr)

        if "[STDERR]" not in p_err.stderr:
            print("❌ Échec: Erreur non redirigée avec préfixe [STDERR].")
            return False

        print("✅ ex2 validé avec succès !")
        return True
    finally:
        shutil.rmtree(temp_dir, ignore_errors=True)


def test_ex3(base_path: Path) -> bool:
    """Teste ex3: ft_vault_security."""
    print("\n" + "=" * 50)
    print("  🧪 Test de ex3/ft_vault_security.py")
    print("=" * 50)

    ex3_dir = base_path / "ex3"
    file_path = ex3_dir / "ft_vault_security.py"
    if not file_path.exists():
        print(f"❌ Fichier manquant: {file_path}")
        return False

    temp_dir = Path(tempfile.mkdtemp(prefix="test_ex3_"))
    try:
        sample_file = temp_dir / "vault_sample.txt"
        sample_file.write_text(
            "CLASSIFIED_ARCHIVE_DATA_2087", encoding="utf-8"
        )

        sys_path_added = str(base_path)
        if sys_path_added not in sys.path:
            sys.path.insert(0, sys_path_added)

        if "ex3.ft_vault_security" in sys.modules:
            del sys.modules["ex3.ft_vault_security"]

        mod = __import__("ex3.ft_vault_security", fromlist=["secure_archive"])
        func = getattr(mod, "secure_archive", None) or getattr(
            mod, "ft_vault_security", None
        )

        if not func:
            print("❌ Aucune fonction 'secure_archive' trouvée dans ex3.")
            return False

        print("→ Test 1: Lecture sécurisée d'un fichier valide")
        res_read = func(str(sample_file), "read")
        print(f"  Retour: {res_read}")
        if not (
            isinstance(res_read, tuple)
            and len(res_read) == 2
            and res_read[0] is True
        ):
            print("❌ Échec: Attendu tuple (True, contenu)")
            return False

        print("\n→ Test 2: Lecture d'un fichier inexistant")
        res_missing = func(str(temp_dir / "nonexistent.txt"), "read")
        print(f"  Retour: {res_missing}")
        if not (
            isinstance(res_missing, tuple)
            and len(res_missing) == 2
            and res_missing[0] is False
        ):
            print("❌ Échec: Attendu tuple (False, message_erreur)")
            return False

        print("\n→ Test 3: Écriture sécurisée")
        new_file = temp_dir / "new_vault.txt"
        res_write = func(str(new_file), "write", "NEW_VAULT_RECORD_BLOCK")
        print(f"  Retour: {res_write}")
        if not (
            isinstance(res_write, tuple)
            and len(res_write) == 2
            and res_write[0] is True
        ):
            print("❌ Échec: Attendu tuple (True, message_succes)")
            return False

        print("\n✅ ex3 validé avec succès !")
        return True
    except Exception as e:
        print(f"❌ Erreur lors de l'exécution de ex3: {e}")
        return False
    finally:
        shutil.rmtree(temp_dir, ignore_errors=True)


def run_tests(choice: str, target_dir: Path) -> bool:
    """Run exercises according to user choice."""
    all_success = True

    if choice == "0":
        all_success &= test_ex0(target_dir)
    elif choice == "1":
        all_success &= test_ex1(target_dir)
    elif choice == "2":
        all_success &= test_ex2(target_dir)
    elif choice == "3":
        all_success &= test_ex3(target_dir)
    elif choice in ("a", "all"):
        for i in range(4):
            all_success &= run_tests(str(i), target_dir)
    else:
        print(f"❌ Choix invalide: {choice}")
        return False

    return all_success


def main():
    target_arg = None
    choice_arg = None
    auto_mode = False

    for arg in sys.argv[1:]:
        if arg in ("--auto", "-a"):
            auto_mode = True
        elif arg in ("--interactive", "-i"):
            auto_mode = False
        elif arg in ("0", "1", "2", "3", "a", "all"):
            choice_arg = arg
        elif Path(arg).is_dir():
            target_arg = arg

    target_dir = find_target_directory(target_arg)
    print("=" * 60)
    print("  🏛️  DATA ARCHIVIST - TEST HELPER RUNNER")
    print(f"  Target:    {target_dir}")
    print(f"  Tests Dir: {TESTS_DIR}")
    print("=" * 60)

    if auto_mode or choice_arg:
        choice = choice_arg if choice_arg else "a"
        success = run_tests(choice, target_dir)
        sys.exit(0 if success else 1)

    while True:
        print("\nChoisissez un exercice à tester :")
        print("  0: ex0 (ft_ancient_text)")
        print("  1: ex1 (ft_archive_creation)")
        print("  2: ex2 (ft_stream_management)")
        print("  3: ex3 (ft_vault_security)")
        print("  a: Tous les exercices")
        print("  q: Quitter")

        try:
            user_choice = input("\nVotre choix [0-3/a/q] : ").strip().lower()
        except (EOFError, KeyboardInterrupt):
            print("\nSortie.")
            break

        if user_choice == "q":
            print("À bientôt !")
            break
        elif user_choice in ("0", "1", "2", "3", "a", "all"):
            run_tests(user_choice, target_dir)
        else:
            print("Option non reconnue, réessayez.")


if __name__ == "__main__":
    main()
