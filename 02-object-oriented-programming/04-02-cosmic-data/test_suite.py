import importlib
import unittest


class TestCosmicData(unittest.TestCase):
    """Suite de tests unitaires pour 04-02-cosmic-data."""

    def test_space_station(self):
        """Vérifie le chargement du module space_station."""
        try:
            m = importlib.import_module('ex0.space_station')
            self.assertIsNotNone(m)
        except ModuleNotFoundError:
            self.skipTest('ex0.space_station not yet implemented')


if __name__ == '__main__':
    unittest.main()
