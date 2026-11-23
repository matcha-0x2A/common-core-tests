#!/usr/bin/env python3
"""
Test runner for 42 Common Core - push_swap
Validates error cases, sorting correctness, operation metrics, and memory leaks.
"""

import sys
import os
import subprocess
import random
import shutil
import argparse
from typing import List, Tuple, Optional

# ANSI colors
RESET = "\033[0m"
BOLD = "\033[1m"
RED = "\033[31m"
GREEN = "\033[32m"
YELLOW = "\033[33m"
BLUE = "\033[34m"
MAGENTA = "\033[35m"
CYAN = "\033[36m"
WHITE = "\033[37m"

class PushSwapChecker:
    """Internal Reference Checker for Push_swap instruction stream."""

    @staticmethod
    def simulate(initial_a: List[int], instructions: str) -> Tuple[bool, str, int]:
        a = list(initial_a)
        b: List[int] = []
        op_count = 0

        for line in instructions.strip().splitlines():
            op = line.strip()
            if not op:
                continue
            op_count += 1
            if op == "sa":
                if len(a) >= 2:
                    a[0], a[1] = a[1], a[0]
            elif op == "sb":
                if len(b) >= 2:
                    b[0], b[1] = b[1], b[0]
            elif op == "ss":
                if len(a) >= 2:
                    a[0], a[1] = a[1], a[0]
                if len(b) >= 2:
                    b[0], b[1] = b[1], b[0]
            elif op == "pa":
                if b:
                    a.insert(0, b.pop(0))
            elif op == "pb":
                if a:
                    b.insert(0, a.pop(0))
            elif op == "ra":
                if len(a) >= 2:
                    a.append(a.pop(0))
            elif op == "rb":
                if len(b) >= 2:
                    b.append(b.pop(0))
            elif op == "rr":
                if len(a) >= 2:
                    a.append(a.pop(0))
                if len(b) >= 2:
                    b.append(b.pop(0))
            elif op == "rra":
                if len(a) >= 2:
                    a.insert(0, a.pop())
            elif op == "rrb":
                if len(b) >= 2:
                    b.insert(0, b.pop())
            elif op == "rrr":
                if len(a) >= 2:
                    a.insert(0, a.pop())
                if len(b) >= 2:
                    b.insert(0, b.pop())
            else:
                return False, f"Instruction inconnue: '{op}'", op_count

        if len(b) != 0:
            return False, f"Pile b non vide ({len(b)} éléments restants)", op_count

        expected_sorted = sorted(initial_a)
        if a != expected_sorted:
            return False, "Pile a non triée en ordre croissant", op_count

        return True, "OK", op_count


class TestRunner:
    def __init__(self, push_swap_bin: str, valgrind_enabled: bool = False):
        self.bin = os.path.abspath(push_swap_bin)
        self.valgrind = valgrind_enabled
        self.total_tests = 0
        self.passed_tests = 0
        self.failed_tests = 0

    def run_cmd(self, args: List[str]) -> Tuple[int, str, str]:
        cmd = [self.bin] + args
        p = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        return p.returncode, p.stdout, p.stderr

    def run_valgrind(self, args: List[str]) -> Tuple[int, str]:
        cmd = [
            "valgrind",
            "--leak-check=full",
            "--show-leak-kinds=all",
            "--errors-for-leak-kinds=all",
            "--error-exitcode=42",
            "-q",
            self.bin
        ] + args
        p = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        return p.returncode, p.stderr

    def assert_test(self, name: str, condition: bool, details: str = ""):
        self.total_tests += 1
        if condition:
            self.passed_tests += 1
            print(f"  {GREEN}✔{RESET} {name}")
        else:
            self.failed_tests += 1
            print(f"  {RED}✖{RESET} {name}")
            if details:
                print(f"    {YELLOW}↳ Détail : {details}{RESET}")

    def test_errors(self):
        print(f"\n{BOLD}{CYAN}=== 1. Tests de Gestion des Erreurs (Norme 42) ==={RESET}")
        
        # Test No argument: doit afficher RIEN et retourner 0
        code, out, err = self.run_cmd([])
        self.assert_test("Aucun argument (doit être silencieux)", code == 0 and out == "" and err == "",
                         f"out='{out}', err='{err}', code={code}")

        error_cases = [
            (["abc"], "Argument non-numérique 'abc'"),
            (["1", "2", "3", "a", "4"], "Token non-numérique au milieu"),
            (["--invalid"], "Flag inconnu"),
            (["+"], "Signe '+' isolé"),
            (["-"], "Signe '-' isolé"),
            (["2147483648"], "Dépassement INT_MAX (+2147483648)"),
            (["-2147483649"], "Dépassement INT_MIN (-2147483649)"),
            (["9999999999999999"], "Grand overflow positif"),
            (["-9999999999999999"], "Grand overflow négatif"),
            (["1", "2", "3", "2"], "Doublon '2'"),
            (["42", "42"], "Doublon '42'"),
            (["0", "1", "2", "0"], "Doublon '0'"),
            (["+42", "42"], "Doublons avec signe '+42' et '42'"),
            ([""], "Chaîne vide \"\""),
            (["   "], "Chaîne d'espaces \"   \""),
        ]

        for args, label in error_cases:
            code, out, err = self.run_cmd(args)
            has_error = ("Error\n" in err or "Error\n" in out or code != 0)
            self.assert_test(f"Erreur attendue : {label}", has_error,
                             f"code={code}, out='{out.strip()}', err='{err.strip()}'")

    def test_identity_and_small(self):
        print(f"\n{BOLD}{CYAN}=== 2. Tests Cas Déjà Triés & Petites Séries (2, 3, 5) ==={RESET}")

        # Cas déjà triés : 0 opération requise
        sorted_cases = [
            (["42"], [42], "1 nombre (42)"),
            (["-42"], [-42], "1 nombre négatif (-42)"),
            (["1", "2"], [1, 2], "2 nombres triés (1 2)"),
            (["1", "2", "3"], [1, 2, 3], "3 nombres triés (1 2 3)"),
            (["1", "2", "3", "4", "5"], [1, 2, 3, 4, 5], "5 nombres triés"),
            ([str(x) for x in range(0, 100, 10)], list(range(0, 100, 10)), "10 nombres triés"),
        ]
        for args, expected, label in sorted_cases:
            code, out, err = self.run_cmd(args)
            lines = [l for l in out.strip().splitlines() if l]
            self.assert_test(f"Déjà trié (0 ops) : {label}", len(lines) == 0,
                             f"Généré {len(lines)} ops au lieu de 0")

        # Cas 2 éléments : max 1 opération
        code, out, err = self.run_cmd(["2", "1"])
        ok, msg, count = PushSwapChecker.simulate([2, 1], out)
        self.assert_test("2 éléments (2 1) : trié en <= 1 op", ok and count <= 1,
                         f"ok={ok}, msg='{msg}', count={count}")

        code, out, err = self.run_cmd(["-10", "-20"])
        ok, msg, count = PushSwapChecker.simulate([-10, -20], out)
        self.assert_test("2 négatifs (-10 -20) : trié en <= 1 op", ok and count <= 1,
                         f"ok={ok}, msg='{msg}', count={count}")

        # Cas 3 éléments : Toutes les 5 permutations non triées doivent être résolues en <= 3 ops
        perm3 = [
            [1, 3, 2],
            [2, 1, 3],
            [2, 3, 1],
            [3, 1, 2],
            [3, 2, 1],
            [0, 2, 1],
            [100, -50, 0],
            [2147483647, -2147483648, 0]
        ]
        p3_success = True
        p3_max = 0
        for p in perm3:
            args = [str(x) for x in p]
            code, out, err = self.run_cmd(args)
            ok, msg, count = PushSwapChecker.simulate(p, out)
            p3_max = max(p3_max, count)
            if not ok or count > 3:
                p3_success = False
                self.assert_test(f"3 éléments ({' '.join(args)})", False,
                                 f"Trié={ok}, ops={count} (max 3 autorisé)")
        if p3_success:
            self.assert_test(f"Toutes permutations de 3 éléments (<= 3 ops, max obtenu: {p3_max})", True)

        # Cas 5 éléments : <= 12 ops
        perm5 = [
            [5, 4, 3, 2, 1],
            [1, 5, 2, 4, 3],
            [2, 3, 5, 1, 4],
            [3, 5, 4, 1, 2],
            [4, 1, 5, 3, 2],
            [-1, -5, -2, -4, -3],
            [0, -100, 100, 50, -50],
        ]
        # Ajouter 5 permutations aléatoires
        for _ in range(5):
            s = list(range(1, 6))
            random.shuffle(s)
            perm5.append(s)

        p5_success = True
        p5_max = 0
        for p in perm5:
            args = [str(x) for x in p]
            code, out, err = self.run_cmd(args)
            ok, msg, count = PushSwapChecker.simulate(p, out)
            p5_max = max(p5_max, count)
            if not ok or count > 12:
                p5_success = False
                self.assert_test(f"5 éléments ({' '.join(args)})", False,
                                 f"Trié={ok}, ops={count} (max 12 autorisé)")
        if p5_success:
            self.assert_test(f"Permutations de 5 éléments (<= 12 ops, max obtenu: {p5_max})", True)

    def test_scale_benchmark(self):
        print(f"\n{BOLD}{CYAN}=== 3. Benchmarks à Grande Échelle (100 et 500 éléments) ==={RESET}")

        # Benchmark 100 éléments (10 itérations)
        counts_100: List[int] = []
        all_100_ok = True
        for i in range(10):
            nums = random.sample(range(-10000, 10000), 100)
            args = [str(x) for x in nums]
            code, out, err = self.run_cmd(args)
            ok, msg, count = PushSwapChecker.simulate(nums, out)
            if not ok:
                all_100_ok = False
                self.assert_test(f"Run 100 elements #{i+1}", False, msg)
            counts_100.append(count)

        min_100 = min(counts_100)
        max_100 = max(counts_100)
        avg_100 = sum(counts_100) / len(counts_100)

        # Barème 42 pour 100 éléments :
        # < 700  : 5 pts
        # < 900  : 4 pts
        # < 1100 : 3 pts
        # < 1300 : 2 pts
        # < 1500 : 1 pt
        pts_100 = 0
        if max_100 < 700:
            pts_100 = 5
        elif max_100 < 900:
            pts_100 = 4
        elif max_100 < 1100:
            pts_100 = 3
        elif max_100 < 1300:
            pts_100 = 2
        elif max_100 < 1500:
            pts_100 = 1

        self.assert_test(f"Taille 100 (10 runs) : Trié à 100%", all_100_ok)
        score_color = GREEN if pts_100 >= 4 else (YELLOW if pts_100 >= 2 else RED)
        print(f"    📊 {BOLD}Stats 100{RESET} : Min = {min_100} | Max = {max_100} | Moyenne = {avg_100:.1f} | Score 42 = {score_color}{pts_100}/5{RESET}")

        # Benchmark 500 éléments (5 itérations)
        counts_500: List[int] = []
        all_500_ok = True
        for i in range(5):
            nums = random.sample(range(-50000, 50000), 500)
            args = [str(x) for x in nums]
            code, out, err = self.run_cmd(args)
            ok, msg, count = PushSwapChecker.simulate(nums, out)
            if not ok:
                all_500_ok = False
                self.assert_test(f"Run 500 elements #{i+1}", False, msg)
            counts_500.append(count)

        min_500 = min(counts_500)
        max_500 = max(counts_500)
        avg_500 = sum(counts_500) / len(counts_500)

        # Barème 42 pour 500 éléments :
        # < 5500  : 5 pts
        # < 7000  : 4 pts
        # < 8500  : 3 pts
        # < 10000 : 2 pts
        # < 11500 : 1 pt
        pts_500 = 0
        if max_500 < 5500:
            pts_500 = 5
        elif max_500 < 7000:
            pts_500 = 4
        elif max_500 < 8500:
            pts_500 = 3
        elif max_500 < 10000:
            pts_500 = 2
        elif max_500 < 11500:
            pts_500 = 1

        self.assert_test(f"Taille 500 (5 runs) : Trié à 100%", all_500_ok)
        score_color = GREEN if pts_500 >= 4 else (YELLOW if pts_500 >= 2 else RED)
        print(f"    📊 {BOLD}Stats 500{RESET} : Min = {min_500} | Max = {max_500} | Moyenne = {avg_500:.1f} | Score 42 = {score_color}{pts_500}/5{RESET}")

    def test_valgrind_suite(self):
        if not self.valgrind:
            return
        if not shutil.which("valgrind"):
            print(f"\n{YELLOW}⚠️  Valgrind n'est pas installé sur la machine. Saut des tests mémoire.{RESET}")
            return

        print(f"\n{BOLD}{CYAN}=== 4. Audit Mémoire Approfondi (Valgrind) ==={RESET}")
        vg_cases = [
            (["1", "2", "3", "a"], "Erreur non-numérique"),
            (["42", "42"], "Erreur doublon"),
            (["2147483648"], "Erreur overflow"),
            (["1", "2", "3", "4", "5"], "Déjà trié (5 éléments)"),
            (["3", "2", "1"], "Tri 3 éléments"),
            (["5", "2", "4", "1", "3"], "Tri 5 éléments"),
            ([str(x) for x in random.sample(range(1000), 100)], "Tri 100 éléments aléatoires"),
        ]

        for args, label in vg_cases:
            code, err = self.run_valgrind(args)
            self.assert_test(f"Valgrind (0 fuite) : {label}", code != 42,
                             f"Valgrind a signalé des erreurs/fuites (code 42 attendu en cas d'erreur mémoire, got {code})\n{err}")

    def run_all(self) -> bool:
        self.test_errors()
        self.test_identity_and_small()
        self.test_scale_benchmark()
        self.test_valgrind_suite()

        print(f"\n{BOLD}{'='*50}{RESET}")
        if self.failed_tests == 0:
            print(f"{GREEN}{BOLD}🎉 TOUS LES TESTS SONT PASSÉS AVEC SUCCÈS ({self.passed_tests}/{self.total_tests}){RESET}")
            return True
        else:
            print(f"{RED}{BOLD}❌ ÉCHECS DÉTECTÉS : {self.failed_tests} sur {self.total_tests} tests.{RESET}")
            return False


def main():
    parser = argparse.ArgumentParser(description="Test Suite pour 42 push_swap")
    parser.add_argument("--bin", default="./push_swap", help="Chemin vers l'exécutable push_swap")
    parser.add_argument("--dir", default=None, help="Chemin vers le dossier push_swap (compilation via make)")
    parser.add_argument("--valgrind", "-v", action="store_true", help="Activer l'audit des fuites mémoire Valgrind")
    args = parser.parse_args()

    target_bin = args.bin
    if args.dir:
        make_dir = os.path.abspath(args.dir)
        print(f"{CYAN}⚙️  Compilation du projet dans {make_dir}...{RESET}")
        p = subprocess.run(["make", "-C", make_dir], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        if p.returncode != 0:
            print(f"{RED}Erreur de compilation du push_swap cible :{RESET}\n{p.stderr}")
            sys.exit(1)
        target_bin = os.path.join(make_dir, "push_swap")

    if not os.path.isfile(target_bin):
        print(f"{RED}❌ Exécutable '{target_bin}' introuvable.{RESET}")
        sys.exit(1)

    runner = TestRunner(target_bin, valgrind_enabled=args.valgrind)
    success = runner.run_all()
    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()
