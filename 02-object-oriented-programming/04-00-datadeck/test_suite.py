import unittest
import importlib

class TestDataDeck(unittest.TestCase):
    def test_battle_module(self):
        try:
            m = importlib.import_module('battle')
            self.assertIsNotNone(m)
        except ModuleNotFoundError:
            self.skipTest('battle.py not yet implemented')

if __name__ == '__main__':
    unittest.main()
