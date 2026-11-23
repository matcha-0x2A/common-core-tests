import importlib
import unittest


class TestDataQuest(unittest.TestCase):
    """Suite de tests unitaires pour 03-00-data-quest."""

    def test_ex0_command_quest(self):
        """Vérifie la présence et le chargement de ft_command_quest."""
        try:
            m = importlib.import_module('ex0.ft_command_quest')
            self.assertTrue(hasattr(m, 'ft_command_quest') or
                            hasattr(m, 'main'))
        except ModuleNotFoundError:
            self.skipTest('ex0 not yet implemented')


if __name__ == '__main__':
    unittest.main()
