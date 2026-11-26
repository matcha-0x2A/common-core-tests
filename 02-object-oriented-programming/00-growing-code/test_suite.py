import contextlib
import importlib
import io
import re
import unittest
from unittest import mock


class TestGrowingCode(unittest.TestCase):
    """Suite de tests unitaires stricts pour 00-growing-code."""

    def test_ex0_hello_garden(self):
        """Vérifie l'affichage exact de ft_hello_garden."""
        try:
            m = importlib.import_module('ex0.ft_hello_garden')
        except ModuleNotFoundError:
            self.skipTest('ex0.ft_hello_garden not yet implemented')

        self.assertTrue(hasattr(m, 'ft_hello_garden'),
                        "La fonction 'ft_hello_garden' doit être définie")
        captured = io.StringIO()
        with contextlib.redirect_stdout(captured):
            m.ft_hello_garden()
        self.assertEqual(
            captured.getvalue().strip(),
            "Hello, Garden Community!",
            "La sortie doit être exactement 'Hello, Garden Community!'"
        )

    def test_ex1_garden_name(self):
        """Vérifie le formatage exact de ft_garden_name."""
        try:
            m = importlib.import_module('ex1.ft_garden_name')
        except ModuleNotFoundError:
            self.skipTest('ex1.ft_garden_name not yet implemented')

        self.assertTrue(hasattr(m, 'ft_garden_name'),
                        "La fonction 'ft_garden_name' doit être définie")
        captured = io.StringIO()
        with mock.patch('builtins.input', return_value='Community Garden'):
            with contextlib.redirect_stdout(captured):
                m.ft_garden_name()
        output = captured.getvalue()
        self.assertTrue(
            bool(re.search(r"Garden:\s{1,2}Community Garden", output)),
            f"Attendu 'Garden: Community Garden', reçu:\n{output}"
        )
        self.assertTrue(
            bool(re.search(r"Status:\s{1,2}Growing well!", output)),
            f"Attendu 'Status: Growing well!', reçu:\n{output}"
        )

    def test_ex2_plot_area(self):
        """Vérifie le calcul et l'affichage de l'aire du potager."""
        try:
            m = importlib.import_module('ex2.ft_plot_area')
        except ModuleNotFoundError:
            self.skipTest('ex2.ft_plot_area not yet implemented')

        self.assertTrue(hasattr(m, 'ft_plot_area'),
                        "La fonction 'ft_plot_area' doit être définie")
        captured = io.StringIO()
        with mock.patch('builtins.input', side_effect=['5', '3']):
            with contextlib.redirect_stdout(captured):
                m.ft_plot_area()
        output = captured.getvalue()
        self.assertTrue(
            bool(re.search(r"Plot area:\s{1,2}15", output)),
            f"Attendu 'Plot area: 15' (1 ou 2 espaces), reçu:\n{output}"
        )

    def test_ex3_harvest_total(self):
        """Vérifie la somme et l'affichage des récoltes."""
        try:
            m = importlib.import_module('ex3.ft_harvest_total')
        except ModuleNotFoundError:
            self.skipTest('ex3.ft_harvest_total not yet implemented')

        self.assertTrue(hasattr(m, 'ft_harvest_total'),
                        "La fonction 'ft_harvest_total' doit être définie")
        captured = io.StringIO()
        with mock.patch('builtins.input', side_effect=['5', '8', '3']):
            with contextlib.redirect_stdout(captured):
                m.ft_harvest_total()
        output = captured.getvalue()
        self.assertTrue(
            bool(re.search(r"Total harvest:\s{1,2}16", output)),
            f"Attendu 'Total harvest: 16' (1 ou 2 espaces), reçu:\n{output}"
        )

    def test_ex4_plant_age(self):
        """Vérifie les conditions de maturité d'une plante."""
        try:
            m = importlib.import_module('ex4.ft_plant_age')
        except ModuleNotFoundError:
            self.skipTest('ex4.ft_plant_age not yet implemented')

        self.assertTrue(hasattr(m, 'ft_plant_age'),
                        "La fonction 'ft_plant_age' doit être définie")

        # Test plante mature (>= 60 jours)
        captured = io.StringIO()
        with mock.patch('builtins.input', return_value='65'):
            with contextlib.redirect_stdout(captured):
                m.ft_plant_age()
        self.assertIn("Plant is ready to harvest!", captured.getvalue())

        # Test plante en croissance (< 60 jours)
        captured = io.StringIO()
        with mock.patch('builtins.input', return_value='30'):
            with contextlib.redirect_stdout(captured):
                m.ft_plant_age()
        self.assertIn("Plant needs more time to grow.", captured.getvalue())

    def test_ex5_water_reminder(self):
        """Vérifie l'alerte d'arrosage."""
        try:
            m = importlib.import_module('ex5.ft_water_reminder')
        except ModuleNotFoundError:
            self.skipTest('ex5.ft_water_reminder not yet implemented')

        self.assertTrue(hasattr(m, 'ft_water_reminder'),
                        "La fonction 'ft_water_reminder' doit être définie")

        # Test arrosage nécessaire (> 2 jours)
        captured = io.StringIO()
        with mock.patch('builtins.input', return_value='4'):
            with contextlib.redirect_stdout(captured):
                m.ft_water_reminder()
        self.assertIn("Water the plants!", captured.getvalue())

        # Test arrosage non requis (<= 2 jours)
        captured = io.StringIO()
        with mock.patch('builtins.input', return_value='1'):
            with contextlib.redirect_stdout(captured):
                m.ft_water_reminder()
        self.assertIn("Plants are fine", captured.getvalue())

    def test_ex6_count_harvest(self):
        """Vérifie le décompte itératif et récursif de la récolte."""
        found = False
        expected_output = [
            "Day 1",
            "Day 2",
            "Day 3",
            "Day 4",
            "Day 5",
            "Harvest time!"
        ]

        try:
            m_it = importlib.import_module('ex6.ft_count_harvest_iterative')
            self.assertTrue(hasattr(m_it, 'ft_count_harvest_iterative'),
                            "ft_count_harvest_iterative doit être définie")
            captured = io.StringIO()
            with mock.patch('builtins.input', return_value='5'):
                with contextlib.redirect_stdout(captured):
                    m_it.ft_count_harvest_iterative()
            raw = captured.getvalue().strip()
            lines = [line.strip() for line in raw.split('\n') if line.strip()]
            self.assertEqual(lines, expected_output,
                             f"Sortie itérative incorrecte:\n{raw}")
            found = True
        except ModuleNotFoundError:
            pass

        try:
            m_rec = importlib.import_module('ex6.ft_count_harvest_recursive')
            self.assertTrue(hasattr(m_rec, 'ft_count_harvest_recursive'),
                            "ft_count_harvest_recursive doit être définie")
            captured = io.StringIO()
            with mock.patch('builtins.input', return_value='5'):
                with contextlib.redirect_stdout(captured):
                    m_rec.ft_count_harvest_recursive()
            raw = captured.getvalue().strip()
            lines = [line.strip() for line in raw.split('\n') if line.strip()]
            self.assertEqual(lines, expected_output,
                             f"Sortie récursive incorrecte:\n{raw}")
            found = True
        except ModuleNotFoundError:
            pass

        if not found:
            self.skipTest('ex6 count_harvest not yet implemented')

    def test_ex7_seed_inventory(self):
        """Vérifie le formatage de l'inventaire de graines selon l'unité."""
        try:
            m = importlib.import_module('ex7.ft_seed_inventory')
        except ModuleNotFoundError:
            self.skipTest('ex7.ft_seed_inventory not yet implemented')

        self.assertTrue(hasattr(m, 'ft_seed_inventory'),
                        "La fonction 'ft_seed_inventory' doit être définie")

        # Test unit: packets
        captured = io.StringIO()
        with contextlib.redirect_stdout(captured):
            m.ft_seed_inventory('tomato', 15, 'packets')
        self.assertTrue(
            bool(re.search(r"Tomato seeds:\s{1,2}15 packets available", captured.getvalue())),
            f"Attendu 'Tomato seeds: 15 packets available' (1 ou 2 espaces), reçu:\n{captured.getvalue()}"
        )

        # Test unit: grams
        captured = io.StringIO()
        with contextlib.redirect_stdout(captured):
            m.ft_seed_inventory('carrot', 50, 'grams')
        self.assertTrue(
            bool(re.search(r"Carrot seeds:\s{1,2}50 grams total", captured.getvalue())),
            f"Attendu 'Carrot seeds: 50 grams total' (1 ou 2 espaces), reçu:\n{captured.getvalue()}"
        )

        # Test unit: area
        captured = io.StringIO()
        with contextlib.redirect_stdout(captured):
            m.ft_seed_inventory('lettuce', 12, 'area')
        self.assertTrue(
            bool(re.search(r"Lettuce seeds:\s{1,2}covers 12 square meters", captured.getvalue())),
            f"Attendu 'Lettuce seeds: covers 12 square meters' (1 ou 2 espaces), reçu:\n{captured.getvalue()}"
        )

        # Test unit inconnue
        captured = io.StringIO()
        with contextlib.redirect_stdout(captured):
            m.ft_seed_inventory('basil', 5, 'boxes')
        self.assertEqual(
            captured.getvalue().strip(),
            "Unknown unit type"
        )


if __name__ == '__main__':
    unittest.main()
