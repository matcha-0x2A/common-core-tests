#include "libft_tests.h"

void test_isalnum(void)
{
	printf("\n--- Testing ft_isalnum ---\n");

	check(ft_isalnum('a') == 1, "ft_isalnum('a') == 1");
	check(ft_isalnum('Z') == 1, "ft_isalnum('Z') == 1");
	check(ft_isalnum('0') == 1, "ft_isalnum('0') == 1");
	check(ft_isalnum('9') == 1, "ft_isalnum('9') == 1");

	check(ft_isalnum('@') == 0, "ft_isalnum('@') == 0");
	check(ft_isalnum('[') == 0, "ft_isalnum('[') == 0");
	check(ft_isalnum('/') == 0, "ft_isalnum('/') == 0");
	check(ft_isalnum(':') == 0, "ft_isalnum(':') == 0");
	check(ft_isalnum(' ') == 0, "ft_isalnum(' ') == 0");
	check(ft_isalnum('!') == 0, "ft_isalnum('!') == 0");
	check(ft_isalnum(-1) == 0, "ft_isalnum(-1) == 0");
}

