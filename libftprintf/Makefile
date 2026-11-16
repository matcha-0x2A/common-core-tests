NAME = run_tests
CC = cc
CFLAGS = -Wall -Wextra -Werror

PRINTF_DIR ?= $(abspath ../../libftprintf)
LIBFTPRINTF = $(PRINTF_DIR)/libftprintf.a

SRCS = printf_tests.c

all: $(NAME)
	@if [ -f "$(PRINTF_DIR)/.wip" ]; then \
		echo "⚠️  [WIP] $(PRINTF_DIR) en cours de développement - tests informatifs"; \
		./$(NAME) || true; \
	else \
		./$(NAME); \
	fi

$(NAME): $(LIBFTPRINTF) $(SRCS)
	@$(CC) $(CFLAGS) $(SRCS) $(LIBFTPRINTF) -I$(PRINTF_DIR) -I. -o $(NAME)

$(LIBFTPRINTF):
	@$(MAKE) -C $(PRINTF_DIR)

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

.PHONY: all valgrind clean fclean re
