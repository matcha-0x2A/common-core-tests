NAME = run_tests
CC = cc
CFLAGS = -Wall -Wextra -Werror
export CCACHE_DISABLE ?= 1

PRINTF_DIR ?= $(shell [ -d "../../../programming-fundamentals/01-00-printf" ] && echo "$$(cd ../../../programming-fundamentals/01-00-printf && pwd)" || echo "$$(cd ../../../libftprintf 2>/dev/null && pwd || echo ../../../libftprintf)")
LIBFTPRINTF = $(PRINTF_DIR)/libftprintf.a

SRCS = printf_tests.c
DEPS = $(wildcard *.h) $(wildcard $(PRINTF_DIR)/*.h)

all: $(NAME)
	@if [ -f "$(PRINTF_DIR)/.wip" ]; then \
		echo "⚠️  [WIP] $(PRINTF_DIR) en cours de développement - tests informatifs"; \
		./$(NAME) || true; \
	else \
		./$(NAME); \
	fi

$(NAME): $(LIBFTPRINTF) $(SRCS) $(DEPS)
	@$(CC) $(CFLAGS) $(SRCS) $(LIBFTPRINTF) -I$(PRINTF_DIR) -I. -o $(NAME)

$(LIBFTPRINTF): FORCE
	@$(MAKE) -C $(PRINTF_DIR)

FORCE:

valgrind: $(NAME)
	@if [ -f "$(PRINTF_DIR)/.wip" ]; then \
		echo "⚠️  [WIP] $(PRINTF_DIR) en cours de développement - valgrind informatif"; \
		valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(NAME) || true; \
	else \
		valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(NAME); \
	fi

clean:
	@rm -f $(NAME)

fclean: clean

re: fclean all

.PHONY: all valgrind clean fclean re FORCE
