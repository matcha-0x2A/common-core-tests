# Root Tests Makefile - Dispatches to project test suites across Common Core

all: help

help:
	@echo "42 Common Core Test Platform"
	@echo "============================"
	@echo ""
	@echo "00-programming-fundamentals (C suites) :"
	@echo "  ./00-programming-fundamentals/test-00-libft.sh [/path/to/00-libft] [--valgrind]"
	@echo "  ./00-programming-fundamentals/test-01-00-printf.sh [/path/to/01-00-printf] [--valgrind]"
	@echo "  ./00-programming-fundamentals/test-01-01-getnextline.sh [/path/to/01-01-getnextline] [--valgrind] [-b BUFFER_SIZE]"
	@echo "  ./00-programming-fundamentals/test-02-push-swap.sh [/path/to/02-push-swap] [--valgrind]"
	@echo ""
	@echo "02-object-oriented-programming (Python suites) :"
	@echo "  ./02-object-oriented-programming/test-<module>.sh [/path/to/<module>]"
	@echo "  ./02-object-oriented-programming/test-00-growing-code-main.sh [--auto|--interactive]"
	@echo "  (Modules : 00-growing-code, 01-code-cultivation, 02-garden-guardian,"
	@echo "             03-00-data-quest, 03-01-data-archivist, 03-02-nexus,"
	@echo "             03-03-the-codex, 04-00-datadeck, 04-01-the-matrix,"
	@echo "             04-02-cosmic-data, 04-03-funcmage)"
	@echo ""
	@echo "Make targets :"
	@echo "  make 00-libft / make libft"
	@echo "  make 01-00-printf / make libftprintf"
	@echo "  make 01-01-getnextline / make getnextline"
	@echo "  make 02-push-swap / make push_swap"
	@echo "  make 00-growing-code (and other python modules)"
	@echo "  make c-tests"
	@echo "  make clean / make fclean"

# C modules
00-libft libft:
	@$(MAKE) -C 00-programming-fundamentals/00-libft

01-00-printf libftprintf:
	@$(MAKE) -C 00-programming-fundamentals/01-00-printf

01-01-getnextline getnextline:
	@$(MAKE) -C 00-programming-fundamentals/01-01-getnextline

02-push-swap push_swap push-swap:
	@$(MAKE) -C 00-programming-fundamentals/02-push-swap

clean:
	@$(MAKE) -C 00-programming-fundamentals/00-libft clean 2>/dev/null || true
	@$(MAKE) -C 00-programming-fundamentals/01-00-printf clean 2>/dev/null || true
	@$(MAKE) -C 00-programming-fundamentals/01-01-getnextline clean 2>/dev/null || true
	@$(MAKE) -C 00-programming-fundamentals/02-push-swap clean 2>/dev/null || true

fclean:
	@$(MAKE) -C 00-programming-fundamentals/00-libft fclean 2>/dev/null || true
	@$(MAKE) -C 00-programming-fundamentals/01-00-printf fclean 2>/dev/null || true
	@$(MAKE) -C 00-programming-fundamentals/01-01-getnextline fclean 2>/dev/null || true
	@$(MAKE) -C 00-programming-fundamentals/02-push-swap fclean 2>/dev/null || true

re: fclean all

# Python modules
OOP_DIR = 02-object-oriented-programming
PYTHON_MODULES = 00-growing-code 01-code-cultivation 02-garden-guardian \
                 03-00-data-quest 03-01-data-archivist 03-02-nexus \
                 03-03-the-codex 04-00-datadeck 04-01-the-matrix \
                 04-02-cosmic-data 04-03-funcmage

$(PYTHON_MODULES):
	@./$(OOP_DIR)/test-$@.sh

00-growing-code-main:
	@./$(OOP_DIR)/test-00-growing-code-main.sh

python-tests: $(PYTHON_MODULES)

c-tests: 00-libft 01-00-printf 01-01-getnextline 02-push-swap

.PHONY: all help clean fclean re c-tests python-tests \
        00-libft libft 01-00-printf libftprintf 01-01-getnextline getnextline \
        02-push-swap push_swap push-swap \
        $(PYTHON_MODULES) 00-growing-code-main
