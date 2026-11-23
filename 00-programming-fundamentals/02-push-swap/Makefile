# Test Runner Makefile for Push_swap

PUSH_SWAP_DIR ?= $(abspath ../../../02-push-swap)

# Autodetect if PUSH_SWAP_DIR does not exist
ifeq ($(wildcard $(PUSH_SWAP_DIR)/Makefile),)
    ifneq ($(wildcard ../../../programming-fundamentals/02-push-swap/Makefile),)
        PUSH_SWAP_DIR = $(abspath ../../../programming-fundamentals/02-push-swap)
    else ifneq ($(wildcard ../../02-push-swap/Makefile),)
        PUSH_SWAP_DIR = $(abspath ../../02-push-swap)
    else ifneq ($(wildcard ../../../push-swap/Makefile),)
        PUSH_SWAP_DIR = $(abspath ../../../push-swap)
    else ifneq ($(wildcard ../../push_swap/Makefile),)
        PUSH_SWAP_DIR = $(abspath ../../push_swap)
    endif
endif

all:
	@chmod +x push_swap_tests.py
	@if [ -f "$(PUSH_SWAP_DIR)/.wip" ]; then \
		echo "⚠️  [WIP] $(PUSH_SWAP_DIR) en cours de développement - tests informatifs"; \
		python3 ./push_swap_tests.py --dir "$(PUSH_SWAP_DIR)" || true; \
	else \
		python3 ./push_swap_tests.py --dir "$(PUSH_SWAP_DIR)"; \
	fi

valgrind:
	@chmod +x push_swap_tests.py
	@if [ -f "$(PUSH_SWAP_DIR)/.wip" ]; then \
		echo "⚠️  [WIP] $(PUSH_SWAP_DIR) en cours de développement - valgrind informatif"; \
		python3 ./push_swap_tests.py --dir "$(PUSH_SWAP_DIR)" --valgrind || true; \
	else \
		python3 ./push_swap_tests.py --dir "$(PUSH_SWAP_DIR)" --valgrind; \
	fi

clean:
	@rm -rf __pycache__ .pytest_cache

fclean: clean

re: fclean all

.PHONY: all valgrind clean fclean re
