import unittest
import importlib

class TestGardenGuardian(unittest.TestCase):
    def test_ex0_first_exception(self):
        try:
            m = importlib.import_module('ex0.ft_first_exception')
            self.assertTrue(hasattr(m, 'ft_first_exception'))
        except ModuleNotFoundError:
            self.skipTest('ex0.ft_first_exception not yet implemented')

if __name__ == '__main__':
    unittest.main()
