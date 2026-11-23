import importlib
import unittest


class TestTheMatrix(unittest.TestCase):
    """Suite de tests unitaires pour 04-01-the-matrix."""

    def test_construct(self):
        """Vérifie le chargement du module construct."""
        try:
            m = importlib.import_module('ex0.construct')
            self.assertIsNotNone(m)
        except ModuleNotFoundError:
            self.skipTest('ex0.construct not yet implemented')


if __name__ == '__main__':
    unittest.main()
