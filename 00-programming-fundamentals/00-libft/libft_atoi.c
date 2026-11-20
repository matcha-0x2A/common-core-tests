#include "libft_tests.h"

static void crash_atoi_null(void)
{
	ft_atoi(NULL);
}

void test_atoi(void)
{
	size_t		i;
	size_t		count;
	char		desc[64];
	const char	*samples[] = {
		"0", "1", "-1", "42", "-42", "+100", "   789", "\t -999",
		"00007", "42abc", "-99xyz", "+0", "-0", "  \n\t +421",
		"", "   ", "abc", "+-1", "--1", "- 5", "+ 5"
	};

	printf("\n--- Testing ft_atoi ---\n");

	/* 1. Basic numbers */
	check_int(ft_atoi("0"), 0, "ft_atoi(\"0\") == 0");
	check_int(ft_atoi("42"), 42, "ft_atoi(\"42\") == 42");
	check_int(ft_atoi("-42"), -42, "ft_atoi(\"-42\") == -42");
	check_int(ft_atoi("+42"), 42, "ft_atoi(\"+42\") == 42");
	check_int(ft_atoi("1"), 1, "ft_atoi(\"1\") == 1");
	check_int(ft_atoi("-1"), -1, "ft_atoi(\"-1\") == -1");
	check_int(ft_atoi("123456789"), 123456789, "ft_atoi(\"123456789\") == 123456789");
	check_int(ft_atoi("-123456789"), -123456789, "ft_atoi(\"-123456789\") == -123456789");

	/* 2. Leading whitespace (spaces, tabs, newlines, vertical tab, form feed, carriage return) */
	check_int(ft_atoi("   42"), 42, "ft_atoi spaces prefix: \"   42\" == 42");
	check_int(ft_atoi("\t\t42"), 42, "ft_atoi tabs prefix: \"\\t\\t42\" == 42");
	check_int(ft_atoi("\n42"), 42, "ft_atoi newline prefix: \"\\n42\" == 42");
	check_int(ft_atoi("\v42"), 42, "ft_atoi vertical tab prefix: \"\\v42\" == 42");
	check_int(ft_atoi("\f42"), 42, "ft_atoi form feed prefix: \"\\f42\" == 42");
	check_int(ft_atoi("\r42"), 42, "ft_atoi carriage return prefix: \"\\r42\" == 42");
	check_int(ft_atoi(" \t\n\v\f\r 42"), 42, "ft_atoi mixed isspace prefix == 42");
	check_int(ft_atoi(" \t\n\v\f\r -42"), -42, "ft_atoi mixed isspace with negative sign == -42");
	check_int(ft_atoi(" \t\n\v\f\r +42"), 42, "ft_atoi mixed isspace with positive sign == 42");

	/* 3. Leading zeros */
	check_int(ft_atoi("0000000"), 0, "ft_atoi(\"0000000\") == 0");
	check_int(ft_atoi("+000042"), 42, "ft_atoi(\"+000042\") == 42");
	check_int(ft_atoi("-000042"), -42, "ft_atoi(\"-000042\") == -42");
	check_int(ft_atoi("+0"), 0, "ft_atoi(\"+0\") == 0");
	check_int(ft_atoi("-0"), 0, "ft_atoi(\"-0\") == 0");

	/* 4. Trailing characters and stop on first non-digit */
	check_int(ft_atoi("42abc"), 42, "ft_atoi stops at letters: \"42abc\" == 42");
	check_int(ft_atoi("-42xyz123"), -42, "ft_atoi stops at letters: \"-42xyz123\" == -42");
	check_int(ft_atoi("1337 42"), 1337, "ft_atoi stops at space after digits: \"1337 42\" == 1337");
	check_int(ft_atoi("42-58"), 42, "ft_atoi stops at second sign: \"42-58\" == 42");
	check_int(ft_atoi("42+58"), 42, "ft_atoi stops at plus sign: \"42+58\" == 42");

	/* 5. Multiple signs and misplaced signs (must return 0) */
	check_int(ft_atoi("--42"), 0, "ft_atoi double minus: \"--42\" == 0");
	check_int(ft_atoi("++42"), 0, "ft_atoi double plus: \"++42\" == 0");
	check_int(ft_atoi("+-42"), 0, "ft_atoi plus-minus: \"+-42\" == 0");
	check_int(ft_atoi("-+42"), 0, "ft_atoi minus-plus: \"-+42\" == 0");
	check_int(ft_atoi(" - - 42"), 0, "ft_atoi space between minus signs: \" - - 42\" == 0");
	check_int(ft_atoi(" + + 42"), 0, "ft_atoi space between plus signs: \" + + 42\" == 0");

	/* 6. Space after sign (must return 0) */
	check_int(ft_atoi("  + 42"), 0, "ft_atoi space after '+': \"  + 42\" == 0");
	check_int(ft_atoi("  - 42"), 0, "ft_atoi space after '-': \"  - 42\" == 0");
	check_int(ft_atoi("+ 1"), 0, "ft_atoi space after '+': \"+ 1\" == 0");
	check_int(ft_atoi("- 1"), 0, "ft_atoi space after '-': \"- 1\" == 0");

	/* 7. Non-numeric or empty strings (must return 0) */
	check_int(ft_atoi(""), 0, "ft_atoi(\"\") == 0");
	check_int(ft_atoi("   "), 0, "ft_atoi whitespace only == 0");
	check_int(ft_atoi("\t\n\v\f\r "), 0, "ft_atoi all isspace characters == 0");
	check_int(ft_atoi("+"), 0, "ft_atoi single '+' == 0");
	check_int(ft_atoi("-"), 0, "ft_atoi single '-' == 0");
	check_int(ft_atoi("abc"), 0, "ft_atoi non-digit string \"abc\" == 0");
	check_int(ft_atoi("+abc"), 0, "ft_atoi \"+abc\" == 0");
	check_int(ft_atoi("-abc"), 0, "ft_atoi \"-abc\" == 0");
	check_int(ft_atoi("   hello 42"), 0, "ft_atoi string before number \"   hello 42\" == 0");

	/* 8. Integer limits (INT_MAX and INT_MIN) */
	check_int(ft_atoi("2147483647"), INT_MAX, "ft_atoi(\"2147483647\") == INT_MAX");
	check_int(ft_atoi("-2147483648"), INT_MIN, "ft_atoi(\"-2147483648\") == INT_MIN");
	check_int(ft_atoi("+2147483647"), INT_MAX, "ft_atoi(\"+2147483647\") == INT_MAX");
	check_int(ft_atoi("2147483646"), INT_MAX - 1, "ft_atoi(\"2147483646\") == INT_MAX - 1");
	check_int(ft_atoi("-2147483647"), INT_MIN + 1, "ft_atoi(\"-2147483647\") == INT_MIN + 1");

	/* 9. Comparison with libc atoi on sample strings */
	count = sizeof(samples) / sizeof(samples[0]);
	for (i = 0; i < count; i++)
	{
		snprintf(desc, sizeof(desc), "ft_atoi matches libc atoi on \"%s\"", samples[i]);
		check_int(ft_atoi(samples[i]), atoi(samples[i]), desc);
	}

	/* 10. Crash test */
	check_crash(crash_atoi_null, "ft_atoi(NULL) raises SIGSEGV (crash attendu)");
}

/* ========================================================================== */
/*                           PART 3: LINKED LISTS                             */
/* ========================================================================== */

