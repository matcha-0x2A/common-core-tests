import contextlib
import importlib
import io
import unittest


class TestCodeCultivation(unittest.TestCase):
    """Suite de tests unitaires pour 01-code-cultivation."""

    def test_ex0_garden_intro(self):
        """Vérifie la structure et l'affichage de ft_garden_intro."""
        try:
            m = importlib.import_module('ex0.ft_garden_intro')
        except ModuleNotFoundError:
            self.skipTest('ex0.ft_garden_intro not yet implemented')

        func = getattr(m, 'ft_garden_intro', None) or getattr(m, 'main', None)
        self.assertIsNotNone(
            func,
            "Le module doit exposer 'ft_garden_intro' ou 'main'"
        )
        captured = io.StringIO()
        with contextlib.redirect_stdout(captured):
            func()
        output = captured.getvalue()
        self.assertIn("=== Welcome to My Garden ===", output)
        self.assertIn("=== End of Program ===", output)
        self.assertIn("Plant:", output)


if __name__ == '__main__':
    unittest.main()
