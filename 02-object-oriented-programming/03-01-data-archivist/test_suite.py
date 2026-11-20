import unittest
import importlib

class TestDataArchivist(unittest.TestCase):
    def test_ex0_ancient_text(self):
        try:
            m = importlib.import_module('ex0.ft_ancient_text')
            self.assertTrue(hasattr(m, 'ft_ancient_text') or hasattr(m, 'main'))
        except ModuleNotFoundError:
            self.skipTest('ex0 not yet implemented')

if __name__ == '__main__':
    unittest.main()
