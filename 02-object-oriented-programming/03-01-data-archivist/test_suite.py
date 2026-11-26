import ast
import importlib
import os
import sys
import unittest


def get_target_dir():
    """Récupère le répertoire cible testé."""
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


class TestDataArchivist(unittest.TestCase):
    """Suite de tests unitaires fonctionnels pour 03-01-data-archivist."""

    def test_ex0_ancient_text(self):
        """Vérifie la présence et le chargement de ft_ancient_text."""
        try:
            m = importlib.import_module('ex0.ft_ancient_text')
            self.assertTrue(hasattr(m, 'ft_ancient_text') or
                            hasattr(m, 'main'))
        except ModuleNotFoundError:
            self.skipTest('ex0 not yet implemented')

    def test_ex1_archive_creation(self):
        """Vérifie la présence et le chargement de ex1."""
        try:
            m = importlib.import_module('ex1.ft_archive_creation')
            self.assertTrue(hasattr(m, 'ft_archive_creation') or
                            hasattr(m, 'main'))
        except ModuleNotFoundError:
            self.skipTest('ex1 not yet implemented')


if __name__ == '__main__':
    unittest.main()
