import importlib
import unittest


class TestFuncMage(unittest.TestCase):
    """Suite de tests unitaires pour 04-03-funcmage."""

    def test_lambda_spells(self):
        """Vérifie le chargement du module lambda_spells."""
        try:
            m = importlib.import_module('ex0.lambda_spells')
            self.assertIsNotNone(m)
        except ModuleNotFoundError:
            self.skipTest('ex0.lambda_spells not yet implemented')


if __name__ == '__main__':
    unittest.main()
