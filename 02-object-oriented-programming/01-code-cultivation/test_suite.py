import unittest
import importlib

class TestCodeCultivation(unittest.TestCase):
    def test_ex0_garden_intro(self):
        try:
            m = importlib.import_module('ex0.ft_garden_intro')
            self.assertTrue(hasattr(m, 'ft_garden_intro') or hasattr(m, 'main'))
        except ModuleNotFoundError:
            self.skipTest('ex0.ft_garden_intro not yet implemented')

if __name__ == '__main__':
    unittest.main()
