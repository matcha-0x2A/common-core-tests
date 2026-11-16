#include "libft_tests.h"

void test_isascii(void)
{
	printf("\n--- Testing ft_isascii ---\n");

	check(ft_isascii(0) == 1, "ft_isascii(0) == 1 (NUL)");
	check(ft_isascii(127) == 1, "ft_isascii(127) == 1 (DEL)");
	check(ft_isascii(65) == 1, "ft_isascii(65) == 1 ('A')");
	check(ft_isascii(32) == 1, "ft_isascii(32) == 1 (' ')");

	check(ft_isascii(-1) == 0, "ft_isascii(-1) == 0");
	check(ft_isascii(-100) == 0, "ft_isascii(-100) == 0");
	check(ft_isascii(128) == 0, "ft_isascii(128) == 0");
	check(ft_isascii(255) == 0, "ft_isascii(255) == 0");
	check(ft_isascii(1000) == 0, "ft_isascii(1000) == 0");
}

