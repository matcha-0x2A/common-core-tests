import importlib
import unittest


class TestGardenGuardian(unittest.TestCase):
    """Suite de tests unitaires pour 02-garden-guardian."""

    def test_ex0_first_exception(self):
        """Vérifie la présence et le chargement de ft_first_exception."""
        try:
            m = importlib.import_module('ex0.ft_first_exception')
            self.assertTrue(hasattr(m, 'ft_first_exception'))
        except ModuleNotFoundError:
            self.skipTest('ex0.ft_first_exception not yet implemented')


if __name__ == '__main__':
    unittest.main()
