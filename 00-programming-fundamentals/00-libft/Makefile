# Test Runner Makefile for Libft

NAME = run_tests
CC = cc
CFLAGS = -Wall -Wextra -Werror
LIBFT_DIR ?= $(abspath ../../libft)
LIBFT = $(LIBFT_DIR)/libft.a

SRCS = $(wildcard libft_*.c)

all: $(NAME)
	@if [ -f "$(LIBFT_DIR)/.wip" ]; then \
		echo "⚠️  [WIP] $(LIBFT_DIR) en cours de développement - tests informatifs"; \
		./$(NAME) || true; \
	else \
		./$(NAME); \
	fi

$(NAME): $(LIBFT) $(SRCS)
	@$(CC) $(CFLAGS) -Wl,--wrap=malloc -Wl,--wrap=free -Wl,--wrap=calloc $(SRCS) $(LIBFT) -I$(LIBFT_DIR) -I. -o $(NAME)

$(LIBFT):
	@$(MAKE) -C $(LIBFT_DIR)

valgrind: $(NAME)
	@if [ -f "$(LIBFT_DIR)/.wip" ]; then \
		echo "⚠️  [WIP] $(LIBFT_DIR) en cours de développement - valgrind informatif"; \
		valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --child-silent-after-fork=yes ./$(NAME) || true; \
	else \
		valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --child-silent-after-fork=yes ./$(NAME); \
	fi

clean:
	@rm -f $(NAME)

fclean: clean

re: fclean all

.PHONY: all clean fclean re valgrind
