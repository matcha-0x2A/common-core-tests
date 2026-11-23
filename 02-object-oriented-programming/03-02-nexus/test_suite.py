import importlib
import unittest


class TestNexus(unittest.TestCase):
    """Suite de tests unitaires pour 03-02-nexus."""

    def test_ex0_data_processor(self):
        """Vérifie la présence et le chargement de DataProcessor."""
        try:
            m = importlib.import_module('ex0.data_processor')
            self.assertTrue(hasattr(m, 'DataProcessor') or
                            hasattr(m, 'process_data'))
        except ModuleNotFoundError:
            self.skipTest('ex0 not yet implemented')


if __name__ == '__main__':
    unittest.main()
