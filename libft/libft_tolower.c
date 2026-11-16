#include "libft_tests.h"

void test_tolower(void)
{
	printf("\n--- Testing ft_tolower ---\n");

	check(ft_tolower('A') == 'a', "ft_tolower('A') == 'a'");
	check(ft_tolower('Z') == 'z', "ft_tolower('Z') == 'z'");
	check(ft_tolower('M') == 'm', "ft_tolower('M') == 'm'");
	check(ft_tolower('a') == 'a', "ft_tolower('a') == 'a'");
	check(ft_tolower('z') == 'z', "ft_tolower('z') == 'z'");
	check(ft_tolower('0') == '0', "ft_tolower('0') == '0'");
	check(ft_tolower(' ') == ' ', "ft_tolower(' ') == ' '");
	check(ft_tolower('@') == '@', "ft_tolower('@') == '@'");
	check(ft_tolower('[') == '[', "ft_tolower('[') == '['");
	check(ft_tolower(-1) == -1, "ft_tolower(-1) == -1");
	check(ft_tolower(128) == 128, "ft_tolower(128) == 128");
}

