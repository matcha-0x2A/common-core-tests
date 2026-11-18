# Root Tests Makefile - Dispatches to project test suites

all: help

help:
	@echo "42 Common Core Test Suites"
	@echo ""
	@echo "Usage via scripts (recommandé) :"
	@echo "  ./test_libft.sh [/chemin/vers/libft] [--valgrind]"
	@echo "  ./test_getnextline.sh [/chemin/vers/gnl] [--valgrind] [-b BUFFER_SIZE]"
	@echo "  ./test_libftprintf.sh [/chemin/vers/printf] [--valgrind]"
	@echo ""
	@echo "Usage direct via make :"
	@echo "  make libft        LIBFT_DIR=/chemin/vers/libft"
	@echo "  make getnextline  GNL_DIR=/chemin/vers/gnl [BUFFER_SIZE=42]"
	@echo "  make libftprintf  PRINTF_DIR=/chemin/vers/printf"
	@echo "  make clean / make fclean"

libft:
	@$(MAKE) -C libft

getnextline:
	@$(MAKE) -C getnextline

libftprintf:
	@$(MAKE) -C libftprintf

clean:
	@$(MAKE) -C libft clean
	@$(MAKE) -C getnextline clean
	@$(MAKE) -C libftprintf clean

fclean:
	@$(MAKE) -C libft fclean
	@$(MAKE) -C getnextline fclean
	@$(MAKE) -C libftprintf fclean

re: fclean all

.PHONY: all help libft getnextline libftprintf clean fclean re
