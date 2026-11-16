#include "libft_tests.h"

void test_isdigit(void)
{
	printf("\n--- Testing ft_isdigit ---\n");

	check(ft_isdigit('0') == 1, "ft_isdigit('0') == 1");
	check(ft_isdigit('5') == 1, "ft_isdigit('5') == 1");
	check(ft_isdigit('9') == 1, "ft_isdigit('9') == 1");

	/* Boundaries and non-digits */
	check(ft_isdigit('0' - 1) == 0, "ft_isdigit('/') == 0");
	check(ft_isdigit('9' + 1) == 0, "ft_isdigit(':') == 0");
	check(ft_isdigit('a') == 0, "ft_isdigit('a') == 0");
	check(ft_isdigit('Z') == 0, "ft_isdigit('Z') == 0");
	check(ft_isdigit(' ') == 0, "ft_isdigit(' ') == 0");
	check(ft_isdigit(-1) == 0, "ft_isdigit(-1) == 0");
	check(ft_isdigit(128) == 0, "ft_isdigit(128) == 0");
}

