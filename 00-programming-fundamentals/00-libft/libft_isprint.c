#include "libft_tests.h"

void test_isprint(void)
{
	printf("\n--- Testing ft_isprint ---\n");

	check(ft_isprint(32) == 1, "ft_isprint(32) == 1 (' ')");
	check(ft_isprint(126) == 1, "ft_isprint(126) == 1 ('~')");
	check(ft_isprint('A') == 1, "ft_isprint('A') == 1");
	check(ft_isprint('0') == 1, "ft_isprint('0') == 1");

	check(ft_isprint(31) == 0, "ft_isprint(31) == 0");
	check(ft_isprint(127) == 0, "ft_isprint(127) == 0 (DEL)");
	check(ft_isprint(0) == 0, "ft_isprint(0) == 0 (NUL)");
	check(ft_isprint('\t') == 0, "ft_isprint('\\t') == 0");
	check(ft_isprint('\n') == 0, "ft_isprint('\\n') == 0");
	check(ft_isprint(-1) == 0, "ft_isprint(-1) == 0");
	check(ft_isprint(128) == 0, "ft_isprint(128) == 0");
}

