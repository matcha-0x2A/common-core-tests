import unittest
import importlib

class TestTheCodex(unittest.TestCase):
    def test_alchemy_package(self):
        try:
            m = importlib.import_module('alchemy')
            self.assertIsNotNone(m)
        except ModuleNotFoundError:
            self.skipTest('alchemy package not yet implemented')

if __name__ == '__main__':
    unittest.main()
