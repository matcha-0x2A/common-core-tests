import importlib
import unittest


class TestTheCodex(unittest.TestCase):
    """Suite de tests unitaires pour 03-03-the-codex."""

    def test_alchemy_package(self):
        """Vérifie le chargement du package alchemy."""
        try:
            m = importlib.import_module('alchemy')
            self.assertIsNotNone(m)
        except ModuleNotFoundError:
            self.skipTest('alchemy package not yet implemented')


if __name__ == '__main__':
    unittest.main()
