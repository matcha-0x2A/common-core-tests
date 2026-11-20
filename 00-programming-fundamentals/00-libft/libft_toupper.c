#include "libft_tests.h"

void test_toupper(void)
{
	printf("\n--- Testing ft_toupper ---\n");

	check(ft_toupper('a') == 'A', "ft_toupper('a') == 'A'");
	check(ft_toupper('z') == 'Z', "ft_toupper('z') == 'Z'");
	check(ft_toupper('m') == 'M', "ft_toupper('m') == 'M'");
	check(ft_toupper('A') == 'A', "ft_toupper('A') == 'A'");
	check(ft_toupper('Z') == 'Z', "ft_toupper('Z') == 'Z'");
	check(ft_toupper('0') == '0', "ft_toupper('0') == '0'");
	check(ft_toupper(' ') == ' ', "ft_toupper(' ') == ' '");
	check(ft_toupper('@') == '@', "ft_toupper('@') == '@'");
	check(ft_toupper('{') == '{', "ft_toupper('{') == '{'");
	check(ft_toupper(-1) == -1, "ft_toupper(-1) == -1");
	check(ft_toupper(128) == 128, "ft_toupper(128) == 128");
}

