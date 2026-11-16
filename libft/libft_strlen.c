#include "libft_tests.h"

void test_strlen(void)
{
	char	long_str[1001];
	int		i;

	printf("\n--- Testing ft_strlen ---\n");

	check(ft_strlen("") == 0, "ft_strlen(\"\") == 0");
	check(ft_strlen("a") == 1, "ft_strlen(\"a\") == 1");
	check(ft_strlen("42") == 2, "ft_strlen(\"42\") == 2");
	check(ft_strlen("Hello, World!") == strlen("Hello, World!"), "ft_strlen(\"Hello, World!\") == strlen()");
	check(ft_strlen("42 Marseille Common Core") == strlen("42 Marseille Common Core"), "ft_strlen standard sentence");

	for (i = 0; i < 1000; i++)
		long_str[i] = 'x';
	long_str[1000] = '\0';
	check(ft_strlen(long_str) == 1000, "ft_strlen(1000 chars) == 1000");
}

