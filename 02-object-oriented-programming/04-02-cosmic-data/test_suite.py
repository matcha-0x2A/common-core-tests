import unittest
import importlib

class TestCosmicData(unittest.TestCase):
    def test_space_station(self):
        try:
            m = importlib.import_module('ex0.space_station')
            self.assertIsNotNone(m)
        except ModuleNotFoundError:
            self.skipTest('ex0.space_station not yet implemented')

if __name__ == '__main__':
    unittest.main()
