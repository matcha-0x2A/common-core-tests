#include "libft_tests.h"

static void check_case(int n, const char *expected, const char *desc)
{
	char	*res;

	res = ft_itoa(n);
	if (!res)
	{
		check(0, desc);
		return ;
	}
	check_str(res, expected, desc);
	free(res);
}

void test_itoa(void)
{
	printf("\n--- Testing ft_itoa ---\n");

	/* 1. Zero */
	check_case(0, "0", "ft_itoa(0) == \"0\"");

	/* 2. Positive single-digit numbers */
	check_case(1, "1", "ft_itoa(1) == \"1\"");
	check_case(5, "5", "ft_itoa(5) == \"5\"");
	check_case(9, "9", "ft_itoa(9) == \"9\"");

	/* 3. Negative single-digit numbers */
	check_case(-1, "-1", "ft_itoa(-1) == \"-1\"");
	check_case(-5, "-5", "ft_itoa(-5) == \"-5\"");
	check_case(-9, "-9", "ft_itoa(-9) == \"-9\"");

	/* 4. Standard positive numbers */
	check_case(42, "42", "ft_itoa(42) == \"42\"");
	check_case(123, "123", "ft_itoa(123) == \"123\"");
	check_case(987654, "987654", "ft_itoa(987654) == \"987654\"");

	/* 5. Standard negative numbers */
	check_case(-42, "-42", "ft_itoa(-42) == \"-42\"");
	check_case(-123, "-123", "ft_itoa(-123) == \"-123\"");
	check_case(-987654, "-987654", "ft_itoa(-987654) == \"-987654\"");

	/* 6. Powers of 10 and multiples of 10 */
	check_case(10, "10", "ft_itoa(10) == \"10\"");
	check_case(100, "100", "ft_itoa(100) == \"100\"");
	check_case(1000000, "1000000", "ft_itoa(1000000) == \"1000000\"");
	check_case(-10, "-10", "ft_itoa(-10) == \"-10\"");
	check_case(-100, "-100", "ft_itoa(-100) == \"-100\"");
	check_case(-1000000, "-1000000", "ft_itoa(-1000000) == \"-1000000\"");

	/* 7. Boundary limits (INT_MAX & INT_MIN) */
	check_case(2147483647, "2147483647", "ft_itoa(INT_MAX) == \"2147483647\"");
	check_case(2147483646, "2147483646", "ft_itoa(INT_MAX - 1) == \"2147483646\"");
	check_case(-2147483648, "-2147483648", "ft_itoa(INT_MIN) == \"-2147483648\"");
	check_case(-2147483647, "-2147483647", "ft_itoa(INT_MIN + 1) == \"-2147483647\"");
}
