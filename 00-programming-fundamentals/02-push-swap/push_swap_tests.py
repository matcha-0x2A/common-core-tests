#!/usr/bin/env python3
"""
Test runner for 42 Common Core - push_swap
Validates all official evaluation sheet questions (Sujet v1.2 - 2026):
  - Question 3 : README.md Compliance Check
  - Question 4 : Norminette
  - Question 5 : Compilation & Makefile (fclean, relink, clean, re)
  - Question 6 : Memory Leaks (Valgrind & Top)
  - Question 7 : Gestion des erreurs
  - Question 8 : Sélection des stratégies (Tests de base)
  - Question 9 : Entrées déjà triées (Identity Test)
  - Question 10 : Petites listes (3 nombres)
  - Question 11 : Moyennes listes (5 nombres)
  - Question 12 : Mode Benchmark & Calcul du désordre
  - Question 13 : Grandes listes (100 nombres)
  - Question 15 : Comparatif des drapeaux de stratégie (50 nombres)
  - Question 16 : Très grandes listes (500 nombres)
  - Question 17 : Live Coding — Ajout du flag --count-only
"""

import sys
import os
import subprocess
import random
import shutil
import argparse
import re
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
    def __init__(self, push_swap_bin: str, project_dir: Optional[str] = None, valgrind_enabled: bool = False):
        self.bin = os.path.abspath(push_swap_bin)
        self.project_dir = os.path.abspath(project_dir) if project_dir else os.path.dirname(self.bin)
        self.valgrind = valgrind_enabled
        self.total_tests = 0
        self.passed_tests = 0
        self.failed_tests = 0

    def run_cmd(self, args: List[str]) -> Tuple[int, str, str]:
        cmd = [self.bin] + args
        p = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        return p.returncode, p.stdout, p.stderr

    def run_valgrind(self, args: List[str]) -> Tuple[int, str, str]:
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
        return p.returncode, p.stdout, p.stderr

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

    def test_readme_compliance(self):
        """Question 3 : README.md Compliance Check"""
        print(f"\n{BOLD}{CYAN}=== Question 3 : README.md Compliance Check ==={RESET}")
        readme_path = os.path.join(self.project_dir, "README.md")
        if not os.path.isfile(readme_path):
            self.assert_test("Présence du fichier README.md à la racine", False, f"Fichier introuvable dans {self.project_dir}")
            return

        self.assert_test("Présence du fichier README.md à la racine", True)

        with open(readme_path, "r", encoding="utf-8", errors="ignore") as f:
            content = f.read()

        lines = [line.strip() for line in content.splitlines() if line.strip()]
        first_line = lines[0] if lines else ""

        # La 1ère ligne doit être en italique (*...*) et mentionner le curriculum 42
        italic_ok = (first_line.startswith("*") and first_line.endswith("*") and
                     "42" in first_line and "curriculum" in first_line.lower())
        self.assert_test("Première ligne en italique conforme (Mention 42 curriculum)", italic_ok,
                         f"Première ligne trouvée : '{first_line}'")

        # Sections obligatoires
        sections = re.findall(r"^##\s+(.*)$", content, flags=re.MULTILINE)
        sections_lower = [s.lower() for s in sections]

        has_description = any("description" in s for s in sections_lower)
        self.assert_test("Section Description présente", has_description)

        has_instructions = any("instruction" in s for s in sections_lower)
        self.assert_test("Section Instructions présente", has_instructions)

        has_resources = any("resource" in s for s in sections_lower)
        self.assert_test("Section Resources présente", has_resources)

        has_contributions = any("contribution" in s or "team" in s or "binôme" in s or "auteurs" in s for s in sections_lower)
        self.assert_test("Section Contributions / Répartition présente", has_contributions)

        # Vérification des 4 stratégies expliquées
        has_algo_strat = (
            ("o(n²)" in content.lower() or "o(n^2)" in content.lower()) and
            ("o(n√n)" in content.lower() or "o(nsqrt(n))" in content.lower()) and
            ("o(n log n)" in content.lower() or "o(nlogn)" in content.lower()) and
            ("adaptive" in content.lower() or "adaptatif" in content.lower())
        )
        self.assert_test("Explication des 4 stratégies (O(n²), O(n√n), O(n log n), adaptatif)", has_algo_strat)

    def test_norminette(self):
        """Question 4 : Norminette"""
        print(f"\n{BOLD}{CYAN}=== Question 4 : Norminette ==={RESET}")
        if not shutil.which("norminette"):
            print(f"  {YELLOW}⚠️  Norminette non installée sur cette machine (saut du test de norme){RESET}")
            return

        c_files = [os.path.join(self.project_dir, f) for f in os.listdir(self.project_dir) if f.endswith(".c") or f.endswith(".h")]
        if not c_files:
            self.assert_test("Fichiers sources C / H trouvés", False, "Aucun .c ou .h dans " + self.project_dir)
            return

        p = subprocess.run(["norminette"] + c_files, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        all_ok = True
        norm_errors = []
        for line in p.stdout.splitlines():
            if line.strip() and not line.endswith("OK!"):
                all_ok = False
                norm_errors.append(line)

        self.assert_test("Norminette sur *.c et *.h (0 erreur)", all_ok, "\n".join(norm_errors[:5]))

    def test_compilation_and_makefile(self):
        """Question 5 : Compilation & Makefile"""
        print(f"\n{BOLD}{CYAN}=== Question 5 : Compilation & Makefile ==={RESET}")
        makefile_path = os.path.join(self.project_dir, "Makefile")
        if not os.path.isfile(makefile_path):
            self.assert_test("Présence du Makefile", False, "Makefile introuvable")
            return

        # 1. make fclean && make
        p_fclean = subprocess.run(["make", "-C", self.project_dir, "fclean"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        p_make = subprocess.run(["make", "-C", self.project_dir], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        self.assert_test("Compilation complète (make fclean && make)", p_make.returncode == 0, p_make.stderr)

        # 2. Vérification de l'absence de relink
        p_relink = subprocess.run(["make", "-C", self.project_dir], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        out_relink = (p_relink.stdout + p_relink.stderr).lower()
        no_relink = ("nothing to be done" in out_relink or "up to date" in out_relink or "à jour" in out_relink or p_relink.stdout.strip() == "")
        self.assert_test("Absence de relink (make consécutif ne recompile rien)", no_relink, p_relink.stdout.strip())

        # 3. Test du nettoyage des objets (make clean)
        p_clean = subprocess.run(["make", "-C", self.project_dir, "clean"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        remaining_o = [f for f in os.listdir(self.project_dir) if f.endswith(".o")]
        self.assert_test("Nettoyage des objets (make clean)", len(remaining_o) == 0, f"Fichiers .o restants : {remaining_o}")

        # 4. Test de recompilation totale (make re)
        p_re = subprocess.run(["make", "-C", self.project_dir, "re"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        self.assert_test("Recompilation totale (make re)", p_re.returncode == 0 and os.path.isfile(self.bin), p_re.stderr)

    def test_errors(self):
        """Question 7 : Gestion des erreurs"""
        print(f"\n{BOLD}{CYAN}=== Question 7 : Gestion des erreurs ==={RESET}")

        # 4. Aucun paramètre : rien d'affiché, code 0
        code, out, err = self.run_cmd([])
        self.assert_test("4. Aucun paramètre : silence absolu et code retour 0",
                         code == 0 and out == "" and err == "",
                         f"out='{out}', err='{err}', code={code}")

        # 1, 2, 3 : Cas explicites du README Question 7
        explicit_q7 = [
            (["1", "2", "a", "3"], "1. Non numérique (./push_swap 1 2 a 3)"),
            (["1", "2", "3", "2"], "2. Doublon (./push_swap 1 2 3 2)"),
            (["2147483648"], "3. Dépassement INT_MAX (./push_swap 2147483648)"),
        ]

        for args, label in explicit_q7:
            code, out, err = self.run_cmd(args)
            # Doit afficher Error\n sur stderr, code != 0, et stdout vide
            is_valid_error = (code != 0 and err.startswith("Error") and out == "")
            self.assert_test(f"{label} (Error sur stderr, code != 0)", is_valid_error,
                             f"code={code}, out='{out.strip()}', err='{err.strip()}'")

        # Cas d'erreurs aux limites additionnels
        additional_errors = [
            (["abc"], "Argument alphabétique 'abc'"),
            (["--invalid"], "Flag inconnu '--invalid'"),
            (["+"], "Signe '+' isolé"),
            (["-"], "Signe '-' isolé"),
            (["-2147483649"], "Dépassement INT_MIN (-2147483649)"),
            (["9999999999999999"], "Grand overflow positif (+9999999999999999)"),
            (["-9999999999999999"], "Grand overflow négatif (-9999999999999999)"),
            (["42", "42"], "Doublon '42'"),
            (["0", "1", "2", "0"], "Doublon '0'"),
            (["+42", "42"], "Doublon avec signe '+42' et '42'"),
            ([""], "Chaîne vide \"\""),
            (["   "], "Chaîne d'espaces \"   \""),
        ]

        for args, label in additional_errors:
            code, out, err = self.run_cmd(args)
            has_error = (code != 0 and ("Error\n" in err or "Error" in err) and out == "")
            self.assert_test(f"Erreur aux limites : {label}", has_error,
                             f"code={code}, out='{out.strip()}', err='{err.strip()}'")

    def test_strategy_selection(self):
        """Question 8 : Sélection des stratégies (Tests de base sur 5 4 3 2 1)"""
        print(f"\n{BOLD}{CYAN}=== Question 8 : Sélection des stratégies (Tests de base) ==={RESET}")
        input_nums = [5, 4, 3, 2, 1]
        args_base = [str(x) for x in input_nums]

        strategies = [
            (["--simple"] + args_base, "Forcer simple (--simple 5 4 3 2 1)"),
            (["--medium"] + args_base, "Forcer medium (--medium 5 4 3 2 1)"),
            (["--complex"] + args_base, "Forcer complex (--complex 5 4 3 2 1)"),
            (["--adaptive"] + args_base, "Forcer adaptive (--adaptive 5 4 3 2 1)"),
            (args_base, "Par défaut sans flag (5 4 3 2 1)"),
        ]

        for cmd_args, label in strategies:
            code, out, err = self.run_cmd(cmd_args)
            ok, msg, count = PushSwapChecker.simulate(input_nums, out)
            self.assert_test(f"{label} -> Tri OK (ops: {count})", code == 0 and ok,
                             f"code={code}, ok={ok}, msg='{msg}', err='{err}'")

    def test_identity(self):
        """Question 9 : Entrées déjà triées (Identity Test)"""
        print(f"\n{BOLD}{CYAN}=== Question 9 : Entrées déjà triées (Identity Test) ==={RESET}")
        identity_cases = [
            (["42"], "1 nombre (42)"),
            (["-42"], "1 nombre négatif (-42)"),
            (["2", "3"], "2 nombres (2 3)"),
            (["0", "1", "2", "3"], "4 nombres (0 1 2 3)"),
            (["0", "1", "2", "3", "4", "5", "6", "7", "8", "9"], "10 nombres (0..9)"),
        ]

        for args, label in identity_cases:
            code, out, err = self.run_cmd(args)
            lines = [l for l in out.strip().splitlines() if l]
            self.assert_test(f"Déjà trié (0 instruction, code 0) : {label}",
                             code == 0 and len(lines) == 0 and err == "",
                             f"code={code}, ops={len(lines)}, out='{out.strip()}', err='{err.strip()}'")

    def test_small_lists_3(self):
        """Question 10 : Petites listes (3 nombres)"""
        print(f"\n{BOLD}{CYAN}=== Question 10 : Petites listes (3 nombres) ==={RESET}")
        explicit_q10 = [
            ([2, 1, 0], "Test 1 (pire cas : 2 1 0)"),
            ([0, 2, 1], "Test 2 (0 2 1)"),
            ([1, 0, 2], "Test 3 (1 0 2)"),
        ]

        for p, label in explicit_q10:
            args = [str(x) for x in p]
            code, out, err = self.run_cmd(args)
            ok, msg, count = PushSwapChecker.simulate(p, out)
            self.assert_test(f"{label} -> Trié en <= 2 ops (obtenu: {count})", ok and count <= 2,
                             f"ok={ok}, ops={count}, msg='{msg}'")

        # Test de l'ensemble des 5 permutations non triées
        all_perm3 = [
            [1, 3, 2],
            [2, 1, 3],
            [2, 3, 1],
            [3, 1, 2],
            [3, 2, 1],
            [-50, 100, 0],
            [2147483647, -2147483648, 0]
        ]
        p3_success = True
        max_ops = 0
        for p in all_perm3:
            args = [str(x) for x in p]
            code, out, err = self.run_cmd(args)
            ok, msg, count = PushSwapChecker.simulate(p, out)
            max_ops = max(max_ops, count)
            if not ok or count > 3:
                p3_success = False
                self.assert_test(f"Permutation 3 éléments ({' '.join(args)})", False, f"ok={ok}, ops={count}")
        if p3_success:
            self.assert_test(f"Toutes les permutations de 3 nombres triées en <= 3 ops (max: {max_ops})", True)

    def test_medium_lists_5(self):
        """Question 11 : Moyennes listes (5 nombres)"""
        print(f"\n{BOLD}{CYAN}=== Question 11 : Moyennes listes (5 nombres) ==={RESET}")
        explicit_q11 = [
            ([1, 5, 2, 4, 3], "Cas du sujet (1 5 2 4 3)"),
            ([5, 1, 4, 2, 3], "Permutation 1 (5 1 4 2 3)"),
            ([3, 5, 1, 4, 2], "Permutation 2 (3 5 1 4 2)"),
        ]

        for p, label in explicit_q11:
            args = [str(x) for x in p]
            code, out, err = self.run_cmd(args)
            ok, msg, count = PushSwapChecker.simulate(p, out)
            self.assert_test(f"{label} -> Trié en <= 12 ops (obtenu: {count})", ok and count <= 12,
                             f"ok={ok}, ops={count}, msg='{msg}'")

        # Permutations aléatoires supplémentaires
        random_perms = [
            [5, 4, 3, 2, 1],
            [2, 3, 5, 1, 4],
            [4, 1, 5, 3, 2],
            [-1, -5, -2, -4, -3],
            [0, -100, 100, 50, -50],
        ]
        for _ in range(5):
            s = list(range(1, 6))
            random.shuffle(s)
            random_perms.append(s)

        p5_success = True
        max_ops = 0
        for p in random_perms:
            args = [str(x) for x in p]
            code, out, err = self.run_cmd(args)
            ok, msg, count = PushSwapChecker.simulate(p, out)
            max_ops = max(max_ops, count)
            if not ok or count > 12:
                p5_success = False
                self.assert_test(f"Permutation 5 éléments ({' '.join(args)})", False, f"ok={ok}, ops={count}")
        if p5_success:
            self.assert_test(f"Toutes les permutations de 5 nombres triées en <= 12 ops (max: {max_ops})", True)

    def test_benchmark_and_disorder(self):
        """Question 12 : Mode Benchmark & Calcul du désordre"""
        print(f"\n{BOLD}{CYAN}=== Question 12 : Mode Benchmark & Calcul du désordre ==={RESET}")

        # Séparation stdout (instructions pures) et stderr (rapport bench 5 lignes)
        code, out, err = self.run_cmd(["--bench", "--simple", "5", "4", "3", "2", "1"])
        
        # 1. stdout ne doit contenir que des opérations valides
        ok_sim, msg_sim, count_sim = PushSwapChecker.simulate([5, 4, 3, 2, 1], out)
        bench_in_stdout = "[bench]" in out
        self.assert_test("Séparation des flux : stdout contient uniquement les instructions valides",
                         code == 0 and ok_sim and not bench_in_stdout,
                         f"bench in stdout={bench_in_stdout}, sim_ok={ok_sim}, msg={msg_sim}")

        # 2. stderr doit contenir le rapport en 5 lignes débutant par [bench]
        err_lines = [l.strip() for l in err.splitlines() if l.strip()]
        has_5_lines = (len(err_lines) == 5 and all(l.startswith("[bench]") for l in err_lines))
        has_disorder = any("[bench] disorder:" in l for l in err_lines)
        has_strategy = any("[bench] strategy:" in l for l in err_lines)
        has_total = any("[bench] total_ops:" in l for l in err_lines)
        has_ops_p1 = any("sa:" in l and "pb:" in l for l in err_lines)
        has_ops_p2 = any("ra:" in l and "rrr:" in l for l in err_lines)

        bench_format_ok = has_5_lines and has_disorder and has_strategy and has_total and has_ops_p1 and has_ops_p2
        self.assert_test("Format du rapport de benchmark (5 lignes précises sur stderr)", bench_format_ok,
                         f"Lignes reçues ({len(err_lines)}):\n" + "\n".join(err_lines))

        # 3. Calcul du désordre sur liste triée (doit afficher 0.00%)
        code, out, err = self.run_cmd(["--bench", "1", "2", "3", "4", "5"])
        disorder_0_ok = any("[bench] disorder: 0.00%" in l for l in err.splitlines())
        self.assert_test("Calcul du désordre sur liste triée 1 2 3 4 5 (0.00%)", disorder_0_ok,
                         f"Sortie stderr:\n{err}")

        # 4. Calcul du désordre sur liste inversée (doit afficher 100.00%)
        code, out, err = self.run_cmd(["--bench", "5", "4", "3", "2", "1"])
        disorder_100_ok = any("[bench] disorder: 100.00%" in l for l in err.splitlines())
        self.assert_test("Calcul du désordre sur liste inversée 5 4 3 2 1 (100.00%)", disorder_100_ok,
                         f"Sortie stderr:\n{err}")

    def test_scale_100(self):
        """Question 13 : Grandes listes (100 nombres)"""
        print(f"\n{BOLD}{CYAN}=== Question 13 : Grandes listes (100 nombres) ==={RESET}")

        # 1. Stratégie --medium (Chunks) -> Attente: 100% EXCELLENT (< 700)
        nums_med = random.sample(range(1, 501), 100)
        args_med = [str(x) for x in nums_med]
        code, out, err = self.run_cmd(["--medium"] + args_med)
        ok, msg, count_med = PushSwapChecker.simulate(nums_med, out)
        is_excellent_med = ok and count_med < 700
        self.assert_test(f"Stratégie --medium (100 nombres) : Excellent < 700 ops (obtenu: {count_med})",
                         is_excellent_med, f"ok={ok}, count={count_med}, msg='{msg}'")

        # 2. Stratégie par défaut (--adaptive) sur 10 runs
        counts_100: List[int] = []
        all_100_ok = True
        for i in range(10):
            nums = random.sample(range(-10000, 10000), 100)
            args = [str(x) for x in nums]
            code, out, err = self.run_cmd(args)
            ok, msg, count = PushSwapChecker.simulate(nums, out)
            if not ok:
                all_100_ok = False
                self.assert_test(f"Run 100 éléments #{i+1}", False, msg)
            counts_100.append(count)

        min_100 = min(counts_100)
        max_100 = max(counts_100)
        avg_100 = sum(counts_100) / len(counts_100)

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

        self.assert_test(f"Taille 100 (10 runs par défaut) : Trié à 100% (max: {max_100} ops)", all_100_ok)
        score_color = GREEN if pts_100 >= 4 else (YELLOW if pts_100 >= 2 else RED)
        print(f"    📊 {BOLD}Stats 100{RESET} : Min = {min_100} | Max = {max_100} | Moyenne = {avg_100:.1f} ops | Barème 42 = {score_color}{pts_100}/5 pts{RESET}")

    def test_strategy_comparison_50(self):
        """Question 15 : Comparatif des drapeaux de stratégie (50 nombres)"""
        print(f"\n{BOLD}{CYAN}=== Question 15 : Comparatif des drapeaux de stratégie (50 nombres) ==={RESET}")
        nums50 = random.sample(range(1, 201), 50)
        args50 = [str(x) for x in nums50]

        strategies = ["--simple", "--medium", "--complex", "--adaptive"]
        counts = {}
        all_sorted = True

        for strat in strategies:
            code, out, err = self.run_cmd([strat] + args50)
            ok, msg, count = PushSwapChecker.simulate(nums50, out)
            counts[strat] = count
            if not ok:
                all_sorted = False
                self.assert_test(f"Tri avec {strat} (50 nombres)", False, msg)

        self.assert_test("Les 4 stratégies trient correctement 50 nombres", all_sorted)
        print(f"    📊 {BOLD}Comparatif des 4 stratégies (50 nombres){RESET} :")
        for strat in strategies:
            name = strat.replace("--", "").capitalize()
            print(f"       • {name:<10} : {counts[strat]} ops")

    def test_scale_500(self):
        """Question 16 : Très grandes listes (500 nombres)"""
        print(f"\n{BOLD}{CYAN}=== Question 16 : Très grandes listes (500 nombres) ==={RESET}")

        # 1. Stratégie --medium (Chunks) -> Attente: 100% EXCELLENT (< 5500)
        nums_med = random.sample(range(1, 1001), 500)
        args_med = [str(x) for x in nums_med]
        code, out, err = self.run_cmd(["--medium"] + args_med)
        ok, msg, count_med = PushSwapChecker.simulate(nums_med, out)
        is_excellent_med = ok and count_med < 5500
        self.assert_test(f"Stratégie --medium (500 nombres) : Excellent < 5500 ops (obtenu: {count_med})",
                         is_excellent_med, f"ok={ok}, count={count_med}, msg='{msg}'")

        # 2. Stratégie par défaut sur 5 runs
        counts_500: List[int] = []
        all_500_ok = True
        for i in range(5):
            nums = random.sample(range(-50000, 50000), 500)
            args = [str(x) for x in nums]
            code, out, err = self.run_cmd(args)
            ok, msg, count = PushSwapChecker.simulate(nums, out)
            if not ok:
                all_500_ok = False
                self.assert_test(f"Run 500 éléments #{i+1}", False, msg)
            counts_500.append(count)

        min_500 = min(counts_500)
        max_500 = max(counts_500)
        avg_500 = sum(counts_500) / len(counts_500)

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

        self.assert_test(f"Taille 500 (5 runs par défaut) : Trié à 100% (max: {max_500} ops)", all_500_ok)
        score_color = GREEN if pts_500 >= 4 else (YELLOW if pts_500 >= 2 else RED)
        print(f"    📊 {BOLD}Stats 500{RESET} : Min = {min_500} | Max = {max_500} | Moyenne = {avg_500:.1f} ops | Barème 42 = {score_color}{pts_500}/5 pts{RESET}")

    def test_live_coding_count_only(self):
        """Question 17 : Live Coding — Option --count-only (Vérification si implémenté)"""
        print(f"\n{BOLD}{CYAN}=== Question 17 : Live Coding — Flag --count-only ==={RESET}")
        code, out, err = self.run_cmd(["--count-only", "3", "2", "1"])
        
        # Si le flag n'est pas encore implémenté (sujet d'épreuve pendant la soutenance)
        if "Error" in err or "Error" in out or code != 0:
            print(f"  {YELLOW}ℹ️  Flag '--count-only' non implémenté (Normal avant la soutenance live coding){RESET}")
            return

        out_clean = out.strip()
        is_num = out_clean.isdigit()
        self.assert_test("Live coding : --count-only affiche uniquement le total numérique (ex: 2 ou 3)",
                         is_num and out_clean in ["2", "3"], f"Sortie reçue: '{out_clean}'")

        code2, out2, err2 = self.run_cmd(["--count-only", "5", "4", "3", "2", "1"])
        self.assert_test("Live coding : --count-only 5 4 3 2 1 affiche uniquement le total",
                         out2.strip().isdigit(), f"Sortie reçue: '{out2.strip()}'")

        code3, out3, err3 = self.run_cmd(["--count-only", "1", "2", "3"])
        self.assert_test("Live coding : --count-only 1 2 3 (déjà trié) affiche 0",
                         out3.strip() == "0", f"Sortie reçue: '{out3.strip()}'")

        code4, out4, err4 = self.run_cmd(["--count-only"])
        self.assert_test("Live coding : --count-only sans argument est silencieux",
                         out4 == "" and err4 == "" and code4 == 0, f"Sortie: out='{out4}', err='{err4}'")

    def test_valgrind_suite(self):
        """Question 6 : Memory Leaks (Valgrind & Top)"""
        if not self.valgrind:
            return
        if not shutil.which("valgrind"):
            print(f"\n{YELLOW}⚠️  Valgrind n'est pas installé sur la machine. Saut des tests mémoire.{RESET}")
            return

        print(f"\n{BOLD}{CYAN}=== Question 6 : Memory Leaks (Valgrind) ==={RESET}")
        vg_cases = [
            (["4", "67", "3", "87", "23"], "Cas nominal Q6 (4 67 3 87 23)"),
            (["--bench"] + [str(x) for x in random.sample(range(1, 1001), 100)], "Mode bench grand jeu Q6 (100 nombres)"),
            (["1", "2", "1"], "Erreur doublon Q6 (1 2 1)"),
            (["2147483648"], "Erreur dépassement INT_MAX Q6 (2147483648)"),
            (["1", "2", "a", "3"], "Erreur non numérique (1 2 a 3)"),
            (["1", "2", "3", "4", "5"], "Cas déjà trié (5 éléments)"),
            (["3", "2", "1"], "Tri 3 éléments"),
            (["5", "2", "4", "1", "3"], "Tri 5 éléments"),
            ([str(x) for x in random.sample(range(1000), 100)], "Tri 100 éléments aléatoires"),
        ]

        for args, label in vg_cases:
            code, out, err = self.run_valgrind(args)
            self.assert_test(f"Valgrind (0 fuite, 0 erreur) : {label}", code != 42,
                             f"Valgrind a signalé des erreurs/fuites (code 42, got {code})\n{err}")

    def run_all(self) -> bool:
        self.test_readme_compliance()
        self.test_norminette()
        self.test_compilation_and_makefile()
        self.test_errors()
        self.test_strategy_selection()
        self.test_identity()
        self.test_small_lists_3()
        self.test_medium_lists_5()
        self.test_benchmark_and_disorder()
        self.test_scale_100()
        self.test_strategy_comparison_50()
        self.test_scale_500()
        self.test_live_coding_count_only()
        self.test_valgrind_suite()

        print(f"\n{BOLD}{'='*50}{RESET}")
        if self.failed_tests == 0:
            print(f"{GREEN}{BOLD}🎉 TOUS LES TESTS DU GUIDE DE SOUTENANCE SONT VALIDÉS ({self.passed_tests}/{self.total_tests}){RESET}")
            return True
        else:
            print(f"{RED}{BOLD}❌ ÉCHECS DÉTECTÉS : {self.failed_tests} sur {self.total_tests} tests.{RESET}")
            return False


def main():
    parser = argparse.ArgumentParser(description="Test Suite Complète pour 42 push_swap (Sujet v1.2)")
    parser.add_argument("--bin", default="./push_swap", help="Chemin vers l'exécutable push_swap")
    parser.add_argument("--dir", default=None, help="Chemin vers le dossier push_swap (compilation via make)")
    parser.add_argument("--valgrind", "-v", action="store_true", help="Activer l'audit des fuites mémoire Valgrind")
    args = parser.parse_args()

    project_dir = None
    target_bin = args.bin
    if args.dir:
        project_dir = os.path.abspath(args.dir)
        print(f"{CYAN}⚙️  Audit et compilation du projet dans {project_dir}...{RESET}")
        p = subprocess.run(["make", "-C", project_dir], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        if p.returncode != 0:
            print(f"{RED}Erreur de compilation du push_swap cible :{RESET}\n{p.stderr}")
            sys.exit(1)
        target_bin = os.path.join(project_dir, "push_swap")
    elif os.path.isfile(args.bin):
        project_dir = os.path.dirname(os.path.abspath(args.bin))

    if not os.path.isfile(target_bin):
        print(f"{RED}❌ Exécutable '{target_bin}' introuvable.{RESET}")
        sys.exit(1)

    runner = TestRunner(target_bin, project_dir=project_dir, valgrind_enabled=args.valgrind)
    success = runner.run_all()
    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()
