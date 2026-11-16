#include "libft_tests.h"

void test_isalpha(void)
{
	printf("\n--- Testing ft_isalpha ---\n");

	/* Subject requires strictly 1 or 0 */
	check(ft_isalpha('a') == 1, "ft_isalpha('a') == 1");
	check(ft_isalpha('z') == 1, "ft_isalpha('z') == 1");
	check(ft_isalpha('m') == 1, "ft_isalpha('m') == 1");
	check(ft_isalpha('A') == 1, "ft_isalpha('A') == 1");
	check(ft_isalpha('Z') == 1, "ft_isalpha('Z') == 1");
	check(ft_isalpha('M') == 1, "ft_isalpha('M') == 1");

	/* Boundaries and non-alpha */
	check(ft_isalpha('A' - 1) == 0, "ft_isalpha('@') == 0");
	check(ft_isalpha('Z' + 1) == 0, "ft_isalpha('[') == 0");
	check(ft_isalpha('a' - 1) == 0, "ft_isalpha('`') == 0");
	check(ft_isalpha('z' + 1) == 0, "ft_isalpha('{') == 0");
	check(ft_isalpha('0') == 0, "ft_isalpha('0') == 0");
	check(ft_isalpha('9') == 0, "ft_isalpha('9') == 0");
	check(ft_isalpha(' ') == 0, "ft_isalpha(' ') == 0");
	check(ft_isalpha('\0') == 0, "ft_isalpha('\\0') == 0");
	check(ft_isalpha(-1) == 0, "ft_isalpha(-1) == 0");
	check(ft_isalpha(128) == 0, "ft_isalpha(128) == 0");
}

