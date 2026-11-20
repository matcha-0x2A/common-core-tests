import unittest
import importlib

class TestTheMatrix(unittest.TestCase):
    def test_construct(self):
        try:
            m = importlib.import_module('ex0.construct')
            self.assertIsNotNone(m)
        except ModuleNotFoundError:
            self.skipTest('ex0.construct not yet implemented')

if __name__ == '__main__':
    unittest.main()
