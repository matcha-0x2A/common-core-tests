import unittest
import importlib

class TestDataQuest(unittest.TestCase):
    def test_ex0_command_quest(self):
        try:
            m = importlib.import_module('ex0.ft_command_quest')
            self.assertTrue(hasattr(m, 'ft_command_quest') or hasattr(m, 'main'))
        except ModuleNotFoundError:
            self.skipTest('ex0 not yet implemented')

if __name__ == '__main__':
    unittest.main()
