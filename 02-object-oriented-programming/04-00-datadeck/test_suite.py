import importlib
import unittest


class TestDataDeck(unittest.TestCase):
    """Suite de tests unitaires pour 04-00-datadeck."""

    def test_battle_module(self):
        """Vérifie le chargement du module battle."""
        try:
            m = importlib.import_module('battle')
            self.assertIsNotNone(m)
        except ModuleNotFoundError:
            self.skipTest('battle.py not yet implemented')


if __name__ == '__main__':
    unittest.main()
