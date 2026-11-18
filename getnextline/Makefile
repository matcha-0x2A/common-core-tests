NAME = run_tests
CC = cc
BUFFER_SIZE ?= 42
CFLAGS = -Wall -Wextra -Werror -D BUFFER_SIZE=$(BUFFER_SIZE)

GNL_DIR ?= $(abspath ../../getnextline)
SRCS = $(GNL_DIR)/get_next_line.c \
       $(GNL_DIR)/get_next_line_utils.c \
       gnl_tests.c

MAIN_NAME = gnl_main
MAIN_SRCS = $(GNL_DIR)/get_next_line.c \
            $(GNL_DIR)/get_next_line_utils.c \
            main.c

all: $(NAME)
	@if [ -f "$(GNL_DIR)/.wip" ]; then \
		echo "⚠️  [WIP] $(GNL_DIR) en cours de développement - tests informatifs"; \
		./$(NAME) || true; \
	else \
		./$(NAME); \
	fi

$(NAME): $(SRCS)
	@$(CC) $(CFLAGS) $(SRCS) -I$(GNL_DIR) -I. -o $(NAME)

main: $(MAIN_NAME)

$(MAIN_NAME): $(MAIN_SRCS)
	@$(CC) $(CFLAGS) $(MAIN_SRCS) -I$(GNL_DIR) -I. -o $(MAIN_NAME)

valgrind: $(NAME)
	@if [ -f "$(GNL_DIR)/.wip" ]; then \
		echo "⚠️  [WIP] $(GNL_DIR) en cours de développement - valgrind informatif"; \
		valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(NAME) || true; \
	else \
		valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(NAME); \
	fi

clean:
	@rm -f $(NAME) $(MAIN_NAME)

fclean: clean

re: fclean all

.PHONY: all valgrind clean fclean re main
