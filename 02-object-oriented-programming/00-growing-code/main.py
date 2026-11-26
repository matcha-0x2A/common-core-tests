#!/usr/bin/env python3

"""
Helper file for Growing Code.

This file helps you test your exercises easily.
Just run: python3 main.py
Or test automatically: python3 main.py --auto
Or test a specific target: python3 main.py /path/to/00-growing-code

How it works:
1. It tries to import your exercise files (like ft_plot_area.py)
2. It calls your functions to test them
3. If there's an error, it tells you what went wrong

Make sure your exercise files are in the target folder!
"""

import os
import sys
from pathlib import Path


# Default test inputs for non-interactive / automated testing
AUTO_INPUTS = {
    "ft_garden_name": ["Community Garden"],
    "ft_plot_area": ["5", "3"],
    "ft_harvest_total": ["10", "20", "30"],
    "ft_plant_age": ["15"],
    "ft_water_reminder": ["3"],
    "ft_count_harvest_iterative": ["5"],
    "ft_count_harvest_recursive": ["5"],
}


def find_target_directory(given_path=None) -> Path:
    """Locate the target directory containing the exercise folders."""
    candidates = []
    if given_path:
        candidates.append(Path(given_path).resolve())
    if "TARGET" in os.environ:
        candidates.append(Path(os.environ["TARGET"]).resolve())
    if "GROWING_CODE_DIR" in os.environ:
        candidates.append(Path(os.environ["GROWING_CODE_DIR"]).resolve())

    # Current working directory if ex0 is present
    cwd = Path.cwd().resolve()
    candidates.append(cwd)

    # Relative to this script parent directory
    script_dir = Path(__file__).resolve().parent
    candidates.append(script_dir)

    # Standard workspace location
    workspace_cand = (
        script_dir / "../../object-oriented-programming/00-growing-code"
    ).resolve()
    candidates.append(workspace_cand)

    for cand in candidates:
        if cand.is_dir() and ((cand / "ex0").exists() or (cand / "dist.files").exists()):
            return cand

    # Default to cwd or script_dir
    return cwd if (cwd / "ex0").exists() else script_dir


def test_ft_exercise(exercise_file_name, folder="", base_path=None, auto_input=False) -> bool:
    """
    This function tries to run one of your exercises.

    For example: test_ft_exercise("ft_plot_area", "ex2") will:
    - Look for a file called ft_plot_area.py in ex2/
    - Import it
    - Call the function ft_plot_area() inside it
    """
    print(f"\n=== Testing {exercise_file_name} ===")

    if base_path is None:
        base_path = find_target_directory()

    try:
        if folder:
            folder_path = str(base_path / folder)
            if folder_path not in sys.path:
                sys.path.insert(0, folder_path)
        else:
            for ex_dir in sorted(base_path.glob("ex*")):
                if (ex_dir / f"{exercise_file_name}.py").exists():
                    sys.path.insert(0, str(ex_dir))
                    break
            else:
                sys.path.append(str(base_path))

        # Clear any previously imported module with the same name
        if exercise_file_name in sys.modules:
            del sys.modules[exercise_file_name]

        ft_module = __import__(exercise_file_name)

        # Get the function from your file
        # This is like doing: ft_plot_area.ft_plot_area
        ft_function = getattr(ft_module, exercise_file_name)

        # Special handling for ft_seed_inventory (Exercise 7)
        # This function takes parameters, unlike the others
        if exercise_file_name == "ft_seed_inventory":
            print("Testing with different seed types and units:\n")
            # Test with packets
            ft_function("tomato", 15, "packets")
            # Test with grams
            ft_function("carrot", 8, "grams")
            # Test with area
            ft_function("lettuce", 12, "area")
            # Test with unknown unit
            print("\nTesting with unknown unit:")
            ft_function("basil", 5, "unknown")
        else:
            # Check if automated inputs should be supplied
            if auto_input and exercise_file_name in AUTO_INPUTS:
                inputs = list(AUTO_INPUTS[exercise_file_name])

                def mock_input(prompt=""):
                    print(prompt, end="")
                    if inputs:
                        val = inputs.pop(0)
                        print(val)
                        return val
                    raise EOFError("No more mock input")

                original_input = __builtins__["input"] if isinstance(__builtins__, dict) else getattr(__builtins__, "input")
                if isinstance(__builtins__, dict):
                    __builtins__["input"] = mock_input
                else:
                    setattr(__builtins__, "input", mock_input)

                try:
                    ft_function()
                finally:
                    if isinstance(__builtins__, dict):
                        __builtins__["input"] = original_input
                    else:
                        setattr(__builtins__, "input", original_input)
            else:
                # Run your function normally (no parameters)
                ft_function()

        return True

    except ImportError:
        target = f"{folder}/" if folder else "its exercise folder"
        print(f"❌ Could not find {exercise_file_name}.py in {target}")
        print("   Make sure your file exists and is in the correct folder.")
        return False

    except AttributeError:
        print(f"❌ Could not find function {exercise_file_name}() in your file")
        print(f"   Make sure you have: def {exercise_file_name}():")
        return False

    except TypeError as error:
        msg = str(error)
        print(f"❌ Type error: {error}")
        if exercise_file_name == "ft_seed_inventory":
            if "missing" in msg and "required positional argument" in msg:
                print(
                    """   For exercise 7, make sure your
                    function takes parameters:"""
                )
                print(
                    f"   def {exercise_file_name}"
                    "(seed_type: str, quantity: int, unit: str) -> None:"
                )
        else:
            print("   Your function should not take any parameters")
        return False

    except Exception as error:
        print(f"❌ Error running your function: {error}")
        print("   Check your code for syntax errors")
        return False


def run_tests(choice: str, target_dir: Path, auto_input: bool = False) -> bool:
    """Run exercises according to user choice."""
    all_success = True

    # Test the exercise based on user choice
    if choice == "0":
        all_success &= test_ft_exercise("ft_hello_garden", "ex0", target_dir, auto_input)
    elif choice == "1":
        all_success &= test_ft_exercise("ft_garden_name", "ex1", target_dir, auto_input)
    elif choice == "2":
        all_success &= test_ft_exercise("ft_plot_area", "ex2", target_dir, auto_input)
    elif choice == "3":
        all_success &= test_ft_exercise("ft_harvest_total", "ex3", target_dir, auto_input)
    elif choice == "4":
        all_success &= test_ft_exercise("ft_plant_age", "ex4", target_dir, auto_input)
    elif choice == "5":
        all_success &= test_ft_exercise("ft_water_reminder", "ex5", target_dir, auto_input)
    elif choice == "6":
        all_success &= test_ft_exercise("ft_count_harvest_iterative", "ex6", target_dir, auto_input)
        all_success &= test_ft_exercise("ft_count_harvest_recursive", "ex6", target_dir, auto_input)
    elif choice == "7":
        all_success &= test_ft_exercise("ft_seed_inventory", "ex7", target_dir, auto_input)
    elif choice in ("a", "all", "--all"):
        all_success &= test_ft_exercise("ft_hello_garden", "ex0", target_dir, auto_input)
        all_success &= test_ft_exercise("ft_garden_name", "ex1", target_dir, auto_input)
        all_success &= test_ft_exercise("ft_plot_area", "ex2", target_dir, auto_input)
        all_success &= test_ft_exercise("ft_harvest_total", "ex3", target_dir, auto_input)
        all_success &= test_ft_exercise("ft_plant_age", "ex4", target_dir, auto_input)
        all_success &= test_ft_exercise("ft_water_reminder", "ex5", target_dir, auto_input)
        all_success &= test_ft_exercise("ft_count_harvest_iterative", "ex6", target_dir, auto_input)
        all_success &= test_ft_exercise("ft_count_harvest_recursive", "ex6", target_dir, auto_input)
        all_success &= test_ft_exercise("ft_seed_inventory", "ex7", target_dir, auto_input)
    else:
        print("❌ Invalid choice! Please enter 0, 1, 2, 3, 4, 5, 6, 7, or a")
        return False

    return all_success


def main():
    """Run main function - supports CLI arguments or interactive prompt."""
    target_arg = None
    choice_arg = None
    auto_input = False

    # Parse arguments
    args = sys.argv[1:]
    for arg in args:
        if arg in ("--auto", "-a", "--batch"):
            auto_input = True
            if choice_arg is None:
                choice_arg = "a"
        elif arg in ("--help", "-h"):
            print("Usage: python3 main.py [/path/to/00-growing-code] [0-7|a] [--auto]")
            sys.exit(0)
        elif os.path.isdir(arg):
            target_arg = arg
        elif arg in [str(i) for i in range(8)] + ["a", "all", "--all"]:
            choice_arg = arg

    target_dir = find_target_directory(target_arg)

    print("🌱 Welcome to Growing Code! 🌱")
    print(f"Target directory: {target_dir}")
    print("This helper will test your exercises for you.")

    if choice_arg is None:
        print("\nWhich exercise would you like to test?")
        print()
        print("0 - ft_hello_garden     (Say hello to the garden community)")
        print("1 - ft_garden_name      (Display garden name)")
        print("2 - ft_plot_area        (Calculate garden plot area)")
        print("3 - ft_harvest_total    (Add up harvest weights)")
        print("4 - ft_plant_age        (Check if plant is ready)")
        print("5 - ft_water_reminder   (Check if plants need water)")
        print("6 - ft_count_harvest    (Count days to harvest)")
        print("7 - ft_seed_inventory   (Seed inventory with type hints)")
        print("a - test all exercises")
        print()

        try:
            choice = input("Enter your choice: ").strip()
        except (EOFError, KeyboardInterrupt):
            print("\nExiting.")
            sys.exit(0)
    else:
        choice = choice_arg

    success = run_tests(choice, target_dir, auto_input=auto_input)
    if not success:
        sys.exit(1)


# This line means: "If someone runs this file directly, call main()"
if __name__ == "__main__":
    main()
