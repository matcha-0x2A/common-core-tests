import importlib
import unittest


class TestDataArchivist(unittest.TestCase):
    """Suite de tests unitaires pour 03-01-data-archivist."""

    def test_ex0_ancient_text(self):
        """Vérifie la présence et le chargement de ft_ancient_text."""
        try:
            m = importlib.import_module('ex0.ft_ancient_text')
            self.assertTrue(hasattr(m, 'ft_ancient_text') or
                            hasattr(m, 'main'))
        except ModuleNotFoundError:
            self.skipTest('ex0 not yet implemented')


if __name__ == '__main__':
    unittest.main()
