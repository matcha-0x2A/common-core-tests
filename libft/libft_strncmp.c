#include "libft_tests.h"

static void crash_strncmp_null_s1(void)
{
	ft_strncmp(NULL, "abc", 3);
}

static void crash_strncmp_null_s2(void)
{
	ft_strncmp("abc", NULL, 3);
}

void test_strncmp(void)
{
	const char	*s_long1 = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	const char	*s_long2 = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

	printf("\n--- Testing ft_strncmp ---\n");

	/* 1. n == 0 (must always return 0, never crash) */
	check_int(ft_strncmp("hello", "world", 0), 0, "ft_strncmp with n = 0 returns 0 on different strings");
	check_int(ft_strncmp("same", "same", 0), 0, "ft_strncmp with n = 0 returns 0 on identical strings");
	check_int(ft_strncmp("", "", 0), 0, "ft_strncmp with n = 0 returns 0 on empty strings");
	check_int(ft_strncmp(NULL, "world", 0), 0, "ft_strncmp(NULL, s2, 0) returns 0 safely without crash");
	check_int(ft_strncmp("hello", NULL, 0), 0, "ft_strncmp(s1, NULL, 0) returns 0 safely without crash");

	/* 2. Identical strings */
	check_int(ft_strncmp("hello", "hello", 5), 0, "ft_strncmp exact length match returns 0");
	check_int(ft_strncmp("hello", "hello", 100), 0, "ft_strncmp n > strlen returns 0");
	check_int(ft_strncmp("", "", 5), 0, "ft_strncmp empty strings return 0");

	/* 3. Prefix matching */
	check_int(ft_strncmp("abcdef", "abcxyz", 3), 0, "ft_strncmp matches prefix of length n returns 0");
	check_cmp(ft_strncmp("abcdef", "abcxyz", 4), strncmp("abcdef", "abcxyz", 4), "ft_strncmp differs at n: 'd' < 'x' returns negative");
	check_int(ft_strncmp("abcdef", "abcxyz", 4), 'd' - 'x', "ft_strncmp exact difference 'd' - 'x'");
	check_cmp(ft_strncmp("abcxyz", "abcdef", 4), strncmp("abcxyz", "abcdef", 4), "ft_strncmp differs at n: 'x' > 'd' returns positive");
	check_int(ft_strncmp("abcxyz", "abcdef", 4), 'x' - 'd', "ft_strncmp exact difference 'x' - 'd'");

	/* 4. Differ at first character */
	check_cmp(ft_strncmp("abc", "bbc", 3), strncmp("abc", "bbc", 3), "ft_strncmp differs at index 0 ('a' < 'b')");
	check_int(ft_strncmp("abc", "bbc", 3), 'a' - 'b', "ft_strncmp exact difference 'a' - 'b'");
	check_cmp(ft_strncmp("bbc", "abc", 3), strncmp("bbc", "abc", 3), "ft_strncmp differs at index 0 ('b' > 'a')");
	check_int(ft_strncmp("bbc", "abc", 3), 'b' - 'a', "ft_strncmp exact difference 'b' - 'a'");

	/* 5. Different lengths */
	check_cmp(ft_strncmp("abc", "abcdef", 5), strncmp("abc", "abcdef", 5), "ft_strncmp shorter s1 vs longer s2 ('\\0' < 'd')");
	check_int(ft_strncmp("abc", "abcdef", 5), '\0' - 'd', "ft_strncmp exact difference '\\0' - 'd'");
	check_cmp(ft_strncmp("abcdef", "abc", 5), strncmp("abcdef", "abc", 5), "ft_strncmp longer s1 vs shorter s2 ('d' > '\\0')");
	check_int(ft_strncmp("abcdef", "abc", 5), 'd' - '\0', "ft_strncmp exact difference 'd' - '\\0'");
	check_int(ft_strncmp("abc", "abcdef", 3), 0, "ft_strncmp n == strlen(shorter) returns 0");

	/* 6. Empty vs non-empty */
	check_cmp(ft_strncmp("", "a", 1), strncmp("", "a", 1), "ft_strncmp empty vs non-empty returns negative");
	check_int(ft_strncmp("", "a", 1), '\0' - 'a', "ft_strncmp exact diff empty vs non-empty");
	check_cmp(ft_strncmp("a", "", 1), strncmp("a", "", 1), "ft_strncmp non-empty vs empty returns positive");
	check_int(ft_strncmp("a", "", 1), 'a' - '\0', "ft_strncmp exact diff non-empty vs empty");

	/* 7. Unsigned char behavior (CRITICAL 42 requirement: characters > 127) */
	check_cmp(ft_strncmp("\200", "\0", 1), 128, "ft_strncmp('\\200', '\\0', 1) is positive (unsigned char)");
	check_int(ft_strncmp("\200", "\0", 1), 128, "ft_strncmp('\\200', '\\0', 1) == 128");
	check_cmp(ft_strncmp("\0", "\200", 1), -128, "ft_strncmp('\\0', '\\200', 1) is negative (unsigned char)");
	check_int(ft_strncmp("\0", "\200", 1), -128, "ft_strncmp('\\0', '\\200', 1) == -128");
	check_cmp(ft_strncmp("test\200", "test\0", 6), 128, "ft_strncmp(\"test\\200\", \"test\\0\", 6) is positive");
	check_int(ft_strncmp("test\200", "test\0", 6), 128, "ft_strncmp(\"test\\200\", \"test\\0\", 6) == 128");
	check_cmp(ft_strncmp("\xff", "\x01", 1), 254, "ft_strncmp('\\xff', '\\x01', 1) == 254");
	check_int(ft_strncmp("\xff", "\x01", 1), 254, "ft_strncmp exact difference 255 - 1");
	check_cmp(ft_strncmp("\x01", "\xff", 1), -254, "ft_strncmp('\\x01', '\\xff', 1) == -254");
	check_int(ft_strncmp("\x01", "\xff", 1), -254, "ft_strncmp exact difference 1 - 255");

	/* 8. Does not compare past '\0' even if n is larger */
	check_int(ft_strncmp("abc\0def", "abc\0xyz", 7), 0, "ft_strncmp stops at '\\0' and returns 0 (ignores post-\\0 diff)");
	check_int(ft_strncmp("1234\0aaaa", "1234\0bbbb", 9), 0, "ft_strncmp stops at '\\0' before diff");

	/* 9. Long strings */
	check_int(ft_strncmp(s_long1, s_long2, strlen(s_long1)), 0, "ft_strncmp long strings exact match");

	/* 10. Crash tests */
	check_crash(crash_strncmp_null_s1, "ft_strncmp(NULL, \"abc\", 3) raises SIGSEGV (crash attendu)");
	check_crash(crash_strncmp_null_s2, "ft_strncmp(\"abc\", NULL, 3) raises SIGSEGV (crash attendu)");
}

/* ========================================================================== */
/*                             ft_memcmp                                      */
/* ========================================================================== */

