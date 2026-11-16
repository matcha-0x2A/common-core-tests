# Root Tests Makefile - Dispatches to project test suites

all: libft

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

.PHONY: all libft getnextline libftprintf clean fclean re
