import ast
import importlib
import os
import shutil
import subprocess
import sys
import tempfile
import unittest


sys.dont_write_bytecode = True

TESTS_DIR = os.path.dirname(os.path.abspath(__file__))
TEST_FRAGMENT_FILE = os.path.join(TESTS_DIR, "ancient_fragment.txt")
TEST_EMPTY_FILE = os.path.join(TESTS_DIR, "empty_fragment.txt")


def get_target_dir():
    """Récupère le répertoire cible du projet testé."""
    target = os.environ.get('TARGET_DIR')
    if target and os.path.isdir(target):
        return os.path.abspath(target)
    for p in sys.path:
        if (p and os.path.isdir(p)
                and not p.startswith(sys.prefix)
                and not p.endswith('site-packages')):
            has_dist = os.path.exists(os.path.join(p, 'dist.files'))
            has_ex = any(os.path.isdir(os.path.join(p, f'ex{i}'))
                         for i in range(4))
            if has_dist or has_ex:
                return os.path.abspath(p)
    return os.path.abspath(os.getcwd())


def find_python_files(target_dir, subdir_prefix=None):
    """Trouve les fichiers .py selon les sous-dossiers spécifiés."""
    py_files = []
    if not os.path.isdir(target_dir):
        return py_files

    for root, dirs, files in os.walk(target_dir):
        dirs[:] = [d for d in dirs
                   if not d.startswith('.') and d != '__pycache__']
        rel_path = os.path.relpath(root, target_dir)
        top_dir = rel_path.split(os.sep)[0]
        if subdir_prefix is not None:
            if top_dir not in subdir_prefix and rel_path not in subdir_prefix:
                continue
        for f in files:
            if f.endswith('.py') and not f.startswith('.'):
                py_files.append(os.path.join(root, f))
    return py_files


def find_script(target_dir, exercise_dir, script_name):
    """Localise le script d'un exercice dans le dossier cible."""
    exact_path = os.path.join(target_dir, exercise_dir, script_name)
    if os.path.isfile(exact_path):
        return exact_path

    ex_path = os.path.join(target_dir, exercise_dir)
    if os.path.isdir(ex_path):
        for f in os.listdir(ex_path):
            if f.endswith('.py') and not f.startswith('.'):
                return os.path.join(ex_path, f)
    return None


def run_script(script_path, argv=None, stdin_text="", cwd=None):
    """Exécute un script CLI dans un sous-processus isolé."""
    if cwd is None:
        cwd = tempfile.gettempdir()
    cmd = [sys.executable, script_path] + (list(argv) if argv else [])
    proc = subprocess.run(
        cmd,
        input=stdin_text,
        text=True,
        capture_output=True,
        cwd=cwd
    )
    return proc.stdout, proc.stderr, proc.returncode


class TestSubjectConstraints(unittest.TestCase):
    """Vérification statique des contraintes pour 03-01-data-archivist."""

    def test_no_with_statement_before_ex3(self):
        """
        Contrainte du sujet:
        "Note for advanced learners: the use of the with statement will be
        introduced in exercise 3. You must not use it before then."
        """
        target_dir = get_target_dir()
        early_dirs = ('ex0', 'ex1', 'ex2')
        py_files = find_python_files(target_dir, early_dirs)

        if not py_files:
            self.skipTest(f"Aucun fichier Python trouvé dans {early_dirs}")

        violations = []
        for file_path in py_files:
            try:
                with open(file_path, 'r', encoding='utf-8',
                          errors='replace') as f:
                    content = f.read()
                tree = ast.parse(content, filename=file_path)
            except Exception as exc:
                violations.append(f"{file_path}: Erreur parsing AST ({exc})")
                continue

            for node in ast.walk(tree):
                if isinstance(node, (ast.With, ast.AsyncWith)):
                    rel_p = os.path.relpath(file_path, target_dir)
                    violations.append(
                        f"{rel_p}:L{node.lineno} - 'with' interdit avant ex3."
                    )

        err_msg = (
            "Violations de la consigne 'with' détectées :\n" +
            "\n".join(f"  ❌ {v}" for v in violations)
        )
        self.assertFalse(violations, err_msg)

    def test_with_statement_present_in_ex3(self):
        """
        Contrainte du sujet:
        L'exercice 3 introduit obligatoirement l'utilisation de l'instruction
        'with' (context manager) pour la sécurité des ressources.
        """
        target_dir = get_target_dir()
        ex3_files = find_python_files(target_dir, ('ex3',))

        if not ex3_files:
            self.skipTest("Aucun fichier Python trouvé dans ex3")

        with_found = False
        for file_path in ex3_files:
            try:
                with open(file_path, 'r', encoding='utf-8',
                          errors='replace') as f:
                    content = f.read()
                tree = ast.parse(content, filename=file_path)
            except Exception:
                continue

            for node in ast.walk(tree):
                if isinstance(node, (ast.With, ast.AsyncWith)):
                    with_found = True
                    break
            if with_found:
                break

        self.assertTrue(
            with_found,
            "L'exercice 3 (ex3) doit obligatoirement utiliser l'instruction "
            "'with' (context manager) pour manipuler les fichiers."
        )

    def test_no_input_builtin_in_ex2(self):
        """
        Contrainte du sujet:
        Dans l'exercice 2, la gestion des flux d'entrée doit s'effectuer
        sans la fonction intégrée input() (ex: via sys.stdin).
        """
        target_dir = get_target_dir()
        ex2_files = find_python_files(target_dir, ('ex2',))

        if not ex2_files:
            self.skipTest("Aucun fichier Python trouvé dans ex2")

        violations = []
        for file_path in ex2_files:
            try:
                with open(file_path, 'r', encoding='utf-8',
                          errors='replace') as f:
                    content = f.read()
                tree = ast.parse(content, filename=file_path)
            except Exception as exc:
                violations.append(f"{file_path}: Erreur parsing ({exc})")
                continue

            for node in ast.walk(tree):
                if isinstance(node, ast.Call):
                    if (isinstance(node.func, ast.Name)
                            and node.func.id == 'input'):
                        rel_p = os.path.relpath(file_path, target_dir)
                        violations.append(
                            f"{rel_p}:L{node.lineno} - Appel interdit à "
                            "'input()'. Utilisez sys.stdin."
                        )

        err_msg = (
            "Violations d'appel à input() dans ex2 détectées :\n" +
            "\n".join(f"  ❌ {v}" for v in violations)
        )
        self.assertFalse(violations, err_msg)

    def test_allowed_standard_types_and_collections(self):
        """
        Contrainte du sujet:
        "The following standard types and collections are allowed:
        str, int, float, list, dict, set, tuple."
        """
        target_dir = get_target_dir()
        py_files = find_python_files(target_dir, ('ex0', 'ex1', 'ex2', 'ex3'))

        if not py_files:
            self.skipTest(f"Aucun fichier Python trouvé sous {target_dir}")

        forbidden_modules = {
            'collections', 'collections.abc', 'heapq', 'bisect',
            'array', 'queue', 'pandas', 'numpy', 'polars'
        }

        violations = []
        for file_path in py_files:
            try:
                with open(file_path, 'r', encoding='utf-8',
                          errors='replace') as f:
                    content = f.read()
                tree = ast.parse(content, filename=file_path)
            except Exception:
                continue

            for node in ast.walk(tree):
                if isinstance(node, ast.Import):
                    for alias in node.names:
                        mod_root = alias.name.split('.')[0]
                        if (mod_root in forbidden_modules
                                or alias.name in forbidden_modules):
                            rel_p = os.path.relpath(file_path, target_dir)
                            violations.append(
                                f"{rel_p}:L{node.lineno} - Import non autorisé"
                                f" '{alias.name}'."
                            )
                elif isinstance(node, ast.ImportFrom):
                    if node.module:
                        mod_root = node.module.split('.')[0]
                        if (mod_root in forbidden_modules
                                or node.module in forbidden_modules):
                            rel_p = os.path.relpath(file_path, target_dir)
                            violations.append(
                                f"{rel_p}:L{node.lineno} - Import non autorisé"
                                f" depuis '{node.module}'."
                            )

        err_msg = (
            "Violations des collections/types autorisés détectées :\n" +
            "\n".join(f"  ❌ {v}" for v in violations)
        )
        self.assertFalse(violations, err_msg)


class TestEx0AncientText(unittest.TestCase):
    """Tests fonctionnels pour ex0: ft_ancient_text."""

    def setUp(self):
        self.target_dir = get_target_dir()
        self.script_path = find_script(self.target_dir, 'ex0',
                                       'ft_ancient_text.py')
        self.temp_dir = tempfile.mkdtemp(prefix="test_ex0_")

    def tearDown(self):
        shutil.rmtree(self.temp_dir, ignore_errors=True)

    def test_ex0_script_presence(self):
        """Vérifie la présence du script ft_ancient_text.py dans ex0."""
        self.assertIsNotNone(
            self.script_path,
            f"ex0/ft_ancient_text.py introuvable dans {self.target_dir}"
        )

    def test_ex0_usage_on_no_arguments(self):
        """Vérifie le message d'utilisation en l'absence d'argument."""
        if not self.script_path:
            self.skipTest("ex0 introuvable")

        out, err, _ = run_script(self.script_path, argv=[], cwd=self.temp_dir)
        combined = (out + err).lower()
        self.assertTrue(
            "usage:" in combined or "ft_ancient_text" in combined,
            f"Un message d'usage est attendu sans argument. Reçu:\n{out + err}"
        )

    def test_ex0_read_existing_fragment(self):
        """Vérifie la lecture du fichier de test ancient_fragment.txt."""
        if not self.script_path:
            self.skipTest("ex0 introuvable")
        self.assertTrue(
            os.path.isfile(TEST_FRAGMENT_FILE),
            f"Fichier de test introuvable: {TEST_FRAGMENT_FILE}"
        )

        out, err, _ = run_script(
            self.script_path,
            argv=[TEST_FRAGMENT_FILE],
            cwd=self.temp_dir
        )
        combined = out + err

        self.assertIn("Cyber Archives", combined,
                      "L'en-tête Cyber Archives doit être affiché")
        self.assertIn(os.path.basename(TEST_FRAGMENT_FILE), combined,
                      "Le nom du fichier traité doit être mentionné")
        self.assertIn("FRAGMENT 001", combined,
                      "Le fragment 001 doit être fidèlement affiché")
        self.assertIn("oblivion", combined,
                      "La fin du texte doit être affichée")
        self.assertTrue(
            "closed" in combined.lower(),
            "La fermeture du fichier doit être signalée"
        )

    def test_ex0_missing_file_handling(self):
        """Vérifie la gestion d'erreur sans crash pour fichier inexistant."""
        if not self.script_path:
            self.skipTest("ex0 introuvable")

        missing_path = os.path.join(self.temp_dir, "nonexistent_vault.txt")
        out, err, _ = run_script(
            self.script_path,
            argv=[missing_path],
            cwd=self.temp_dir
        )
        combined = (out + err).lower()
        has_error = any(k in combined for k in ("error", "not found",
                                                "errno 2", "errno"))
        self.assertTrue(
            has_error,
            f"Une erreur explicite doit être affichée. Reçu:\n{out + err}"
        )


class TestEx1ArchiveCreation(unittest.TestCase):
    """Tests fonctionnels pour ex1: ft_archive_creation."""

    def setUp(self):
        self.target_dir = get_target_dir()
        self.script_path = find_script(self.target_dir, 'ex1',
                                       'ft_archive_creation.py')
        self.temp_dir = tempfile.mkdtemp(prefix="test_ex1_")

    def tearDown(self):
        shutil.rmtree(self.temp_dir, ignore_errors=True)

    def test_ex1_script_presence(self):
        """Vérifie la présence du script ft_archive_creation.py dans ex1."""
        self.assertIsNotNone(
            self.script_path,
            f"ex1/ft_archive_creation.py introuvable dans {self.target_dir}"
        )

    def test_ex1_usage_on_no_arguments(self):
        """Vérifie l'affichage de l'usage en l'absence d'arguments."""
        if not self.script_path:
            self.skipTest("ex1 introuvable")

        out, err, _ = run_script(self.script_path, argv=[], cwd=self.temp_dir)
        combined = (out + err).lower()
        self.assertTrue(
            "usage:" in combined or "ft_" in combined,
            f"Usage attendu en l'absence d'arguments. Reçu:\n{out + err}"
        )

    def test_ex1_read_transform_and_save(self):
        """Vérifie la transformation et la sauvegarde d'archive."""
        if not self.script_path:
            self.skipTest("ex1 introuvable")

        output_name = "preserved_archive.txt"
        out, err, _ = run_script(
            self.script_path,
            argv=[TEST_FRAGMENT_FILE],
            stdin_text=f"{output_name}\n",
            cwd=self.temp_dir
        )
        combined = out + err

        self.assertIn("Cyber Archives", combined,
                      "L'en-tête Cyber Archives doit être affiché")
        self.assertIn("Transform data:", combined,
                      "La section de transformation doit être affichée")
        self.assertIn("#", combined,
                      "Les données transformées doivent comporter '#'")
        self.assertTrue(
            "saved" in combined.lower(),
            "La confirmation de sauvegarde doit être affichée"
        )

        saved_path = os.path.join(self.temp_dir, output_name)
        self.assertTrue(
            os.path.isfile(saved_path),
            f"Le fichier sauvegardé {saved_path} doit exister sur le disque"
        )
        with open(saved_path, "r", encoding="utf-8") as f:
            content = f.read()
        self.assertIn("#", content,
                      "Le contenu sauvegardé doit contenir les modifications")

    def test_ex1_cancel_save_when_empty_filename(self):
        """Vérifie l'annulation de la sauvegarde sur nom vide."""
        if not self.script_path:
            self.skipTest("ex1 introuvable")

        out, err, _ = run_script(
            self.script_path,
            argv=[TEST_FRAGMENT_FILE],
            stdin_text="\n",
            cwd=self.temp_dir
        )
        combined = (out + err).lower()
        self.assertTrue(
            "not saving" in combined or "cancel" in combined,
            f"Message d'annulation attendu sur nom vide. Reçu:\n{out + err}"
        )

    def test_ex1_missing_file_handling(self):
        """Vérifie la gestion d'erreur si le fichier source est introuvable."""
        if not self.script_path:
            self.skipTest("ex1 introuvable")

        missing = os.path.join(self.temp_dir, "unknown_vault.txt")
        out, err, _ = run_script(
            self.script_path,
            argv=[missing],
            stdin_text="\n",
            cwd=self.temp_dir
        )
        combined = (out + err).lower()
        has_err = any(k in combined for k in ("error", "not found", "errno"))
        self.assertTrue(
            has_err,
            f"Message d'erreur attendu si fichier manquant. Reçu:\n{out + err}"
        )


class TestEx2StreamManagement(unittest.TestCase):
    """Tests fonctionnels pour ex2: ft_stream_management."""

    def setUp(self):
        self.target_dir = get_target_dir()
        self.script_path = find_script(self.target_dir, 'ex2',
                                       'ft_stream_management.py')
        self.temp_dir = tempfile.mkdtemp(prefix="test_ex2_")

    def tearDown(self):
        shutil.rmtree(self.temp_dir, ignore_errors=True)

    def test_ex2_script_presence(self):
        """Vérifie la présence du script ft_stream_management.py dans ex2."""
        self.assertIsNotNone(
            self.script_path,
            f"ex2/ft_stream_management.py introuvable dans {self.target_dir}"
        )

    def test_ex2_stdout_normal_operation(self):
        """Vérifie que les logs standards sont émis sur stdout sans erreur."""
        if not self.script_path:
            self.skipTest("ex2 introuvable")

        out, err, _ = run_script(
            self.script_path,
            argv=[TEST_FRAGMENT_FILE],
            stdin_text="\n",
            cwd=self.temp_dir
        )
        self.assertIn("Cyber Archives", out,
                      "Les messages normaux doivent être émis sur stdout")
        self.assertIn("FRAGMENT 001", out,
                      "Le contenu lu doit être présent sur stdout")
        self.assertEqual(
            err.strip(), "",
            f"Aucune erreur ne doit apparaître sur stderr. Reçu:\n{err}"
        )

    def test_ex2_stderr_error_redirection(self):
        """Vérifie que les erreurs sont émises sur sys.stderr avec [STDERR]."""
        if not self.script_path:
            self.skipTest("ex2 introuvable")

        missing_file = os.path.join(self.temp_dir, "corrupted_archive.txt")
        out, err, _ = run_script(
            self.script_path,
            argv=[missing_file],
            stdin_text="\n",
            cwd=self.temp_dir
        )

        self.assertTrue(
            len(err) > 0,
            f"Une erreur doit être émise sur stderr. Reçu stdout:\n{out}"
        )
        self.assertIn(
            "[STDERR]", err,
            f"Le marqueur [STDERR] doit être présent sur stderr. Reçu:\n{err}"
        )

    def test_ex2_save_stream(self):
        """Vérifie la sauvegarde via stream management."""
        if not self.script_path:
            self.skipTest("ex2 introuvable")

        out_name = "stream_output.txt"
        out, err, _ = run_script(
            self.script_path,
            argv=[TEST_FRAGMENT_FILE],
            stdin_text=f"{out_name}\n",
            cwd=self.temp_dir
        )
        saved_file = os.path.join(self.temp_dir, out_name)
        self.assertTrue(
            os.path.isfile(saved_file),
            f"Le fichier stream {saved_file} doit avoir été créé"
        )


class TestEx3VaultSecurity(unittest.TestCase):
    """Tests fonctionnels pour ex3: ft_vault_security."""

    def setUp(self):
        self.target_dir = get_target_dir()
        self.temp_dir = tempfile.mkdtemp(prefix="test_ex3_")
        self.sample_file = os.path.join(self.temp_dir, "vault_data.txt")
        self.content = "CLASSIFIED_ARCHIVE_DATA_2087"
        with open(self.sample_file, "w", encoding="utf-8") as f:
            f.write(self.content)

        if self.target_dir not in sys.path:
            sys.path.insert(0, self.target_dir)

    def tearDown(self):
        shutil.rmtree(self.temp_dir, ignore_errors=True)

    def _get_secure_archive_func(self):
        try:
            m = importlib.import_module('ex3.ft_vault_security')
            return getattr(m, 'secure_archive', None) or getattr(
                m, 'ft_vault_security', None
            )
        except ModuleNotFoundError:
            return None

    def test_ex3_module_and_function_presence(self):
        """Vérifie la présence de secure_archive dans ex3."""
        func = self._get_secure_archive_func()
        self.assertIsNotNone(
            func,
            "ex3 doit définir la fonction 'secure_archive'"
        )

    def test_ex3_type_annotations(self):
        """Vérifie la présence de type hints conformes PEP 484."""
        func = self._get_secure_archive_func()
        if func is None:
            self.skipTest("secure_archive non trouvé")

        annotations = getattr(func, '__annotations__', {})
        self.assertTrue(
            len(annotations) > 0,
            f"La fonction {func.__name__} doit comporter des type hints."
        )

    def test_ex3_read_success(self):
        """Vérifie la lecture sécurisée et le retour (True, content)."""
        func = self._get_secure_archive_func()
        if func is None:
            self.skipTest("secure_archive non trouvé")

        res = func(self.sample_file, 'read')
        self.assertIsInstance(res, tuple,
                              "Doit renvoyer un tuple (bool, str)")
        self.assertEqual(len(res), 2,
                         "Le tuple de retour doit contenir 2 éléments")
        self.assertTrue(
            res[0],
            "Statut de lecture d'un fichier valide doit être True"
        )
        self.assertEqual(res[1], self.content,
                         "Contenu lu doit être identique au fichier")

    def test_ex3_read_missing_file(self):
        """Vérifie la lecture d'un fichier inexistant -> (False, error_msg)."""
        func = self._get_secure_archive_func()
        if func is None:
            self.skipTest("secure_archive non trouvé")

        missing_path = os.path.join(self.temp_dir, "missing_vault.txt")
        res = func(missing_path, 'read')

        self.assertIsInstance(res, tuple)
        self.assertFalse(res[0],
                         "Statut d'un fichier inexistant doit être False")
        self.assertIsInstance(res[1], str,
                              "Le message d'erreur doit être un str")
        self.assertTrue(
            len(res[1]) > 0,
            "Le message d'erreur ne doit pas être vide"
        )

    def test_ex3_write_success(self):
        """Vérifie l'écriture sécurisée et le retour (True, confirmation)."""
        func = self._get_secure_archive_func()
        if func is None:
            self.skipTest("secure_archive non trouvé")

        new_vault = os.path.join(self.temp_dir, "new_vault_block.txt")
        new_content = "SECURE_PROTOCOL_INITIATED"

        res = func(new_vault, 'write', new_content)
        self.assertIsInstance(res, tuple)
        self.assertTrue(res[0], "L'écriture doit retourner True en premier")

        self.assertTrue(os.path.isfile(new_vault),
                        "Le fichier écrit doit exister sur le disque")
        with open(new_vault, 'r', encoding='utf-8') as f:
            written_data = f.read()
        self.assertEqual(written_data, new_content,
                         "Données écrites doivent correspondre au paramètre")

    def test_ex3_invalid_action(self):
        """Vérifie le comportement en cas d'action invalide."""
        func = self._get_secure_archive_func()
        if func is None:
            self.skipTest("secure_archive non trouvé")

        res = func(self.sample_file, 'invalid_action_xyz')
        self.assertIsInstance(res, tuple)
        self.assertFalse(res[0],
                         "Une action inconnue doit retourner (False, ...)")


if __name__ == '__main__':
    unittest.main()
