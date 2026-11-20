import unittest
import importlib
import sys
import io

class TestGrowingCode(unittest.TestCase):
    def test_ex0_hello_garden(self):
        try:
            m = importlib.import_module('ex0.ft_hello_garden')
            self.assertTrue(hasattr(m, 'ft_hello_garden'))
            captured = io.StringIO()
            sys.stdout = captured
            m.ft_hello_garden()
            sys.stdout = sys.__stdout__
            self.assertTrue(len(captured.getvalue().strip()) > 0)
        except ModuleNotFoundError:
            self.skipTest('ex0.ft_hello_garden not yet implemented')

    def test_ex7_seed_inventory(self):
        try:
            m = importlib.import_module('ex7.ft_seed_inventory')
            self.assertTrue(hasattr(m, 'ft_seed_inventory'))
            captured = io.StringIO()
            sys.stdout = captured
            m.ft_seed_inventory('tomato', 15, 'packets')
            sys.stdout = sys.__stdout__
            self.assertIn('15', captured.getvalue())
        except ModuleNotFoundError:
            self.skipTest('ex7.ft_seed_inventory not yet implemented')

if __name__ == '__main__':
    unittest.main()
