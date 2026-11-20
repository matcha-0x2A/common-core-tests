import unittest
import importlib

class TestFuncMage(unittest.TestCase):
    def test_lambda_spells(self):
        try:
            m = importlib.import_module('ex0.lambda_spells')
            self.assertIsNotNone(m)
        except ModuleNotFoundError:
            self.skipTest('ex0.lambda_spells not yet implemented')

if __name__ == '__main__':
    unittest.main()
