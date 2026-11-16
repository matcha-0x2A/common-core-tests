#include "libft_tests.h"

static void crash_memcmp_null_s1(void)
{
	ft_memcmp(NULL, "abc", 3);
}

static void crash_memcmp_null_s2(void)
{
	ft_memcmp("abc", NULL, 3);
}

void test_memcmp(void)
{
	int	arr1[3] = {1, 2, 3};
	int	arr2[3] = {1, 2, 3};
	int	arr3[3] = {1, 2, 4};

	printf("\n--- Testing ft_memcmp ---\n");

	/* 1. n == 0 */
	check_int(ft_memcmp("hello", "world", 0), 0, "ft_memcmp with n = 0 returns 0 on different buffers");
	check_int(ft_memcmp("same", "same", 0), 0, "ft_memcmp with n = 0 returns 0 on identical buffers");
	check_int(ft_memcmp("", "", 0), 0, "ft_memcmp with n = 0 returns 0 on empty strings");
	check_int(ft_memcmp(NULL, "world", 0), 0, "ft_memcmp(NULL, s2, 0) returns 0 safely without crash");
	check_int(ft_memcmp("hello", NULL, 0), 0, "ft_memcmp(s1, NULL, 0) returns 0 safely without crash");

	/* 2. Identical buffers */
	check_int(ft_memcmp("hello", "hello", 5), 0, "ft_memcmp identical buffers return 0");
	check_int(ft_memcmp("abc\0def", "abc\0def", 7), 0, "ft_memcmp identical buffers with embedded \\0 return 0");

	/* 3. Differ before null byte */
	check_cmp(ft_memcmp("abcdef", "abcxyz", 4), memcmp("abcdef", "abcxyz", 4), "ft_memcmp differs at index 3 ('d' < 'x')");
	check_int(ft_memcmp("abcdef", "abcxyz", 4), 'd' - 'x', "ft_memcmp exact difference 'd' - 'x'");
	check_cmp(ft_memcmp("abcxyz", "abcdef", 4), memcmp("abcxyz", "abcdef", 4), "ft_memcmp differs at index 3 ('x' > 'd')");
	check_int(ft_memcmp("abcxyz", "abcdef", 4), 'x' - 'd', "ft_memcmp exact difference 'x' - 'd'");

	/* 4. Differ at first byte */
	check_cmp(ft_memcmp("abc", "bbc", 3), memcmp("abc", "bbc", 3), "ft_memcmp differs at index 0 ('a' < 'b')");
	check_int(ft_memcmp("abc", "bbc", 3), 'a' - 'b', "ft_memcmp exact diff 'a' - 'b'");

	/* 5. CRITICAL: Differ AFTER '\0' (memcmp MUST compare past '\0') */
	check_cmp(ft_memcmp("abc\0def", "abc\0xyz", 7), memcmp("abc\0def", "abc\0xyz", 7),
		"ft_memcmp compares past '\\0': 'd' < 'x' returns negative");
	check_int(ft_memcmp("abc\0def", "abc\0xyz", 7), 'd' - 'x', "ft_memcmp exact diff past '\\0' ('d' - 'x')");
	check_cmp(ft_memcmp("abc\0xyz", "abc\0def", 7), memcmp("abc\0xyz", "abc\0def", 7),
		"ft_memcmp compares past '\\0': 'x' > 'd' returns positive");
	check_int(ft_memcmp("abc\0xyz", "abc\0def", 7), 'x' - 'd', "ft_memcmp exact diff past '\\0' ('x' - 'd')");

	/* 6. Difference is past n (must return 0) */
	check_int(ft_memcmp("abcdef", "abcdeg", 5), 0, "ft_memcmp difference at byte 6 ignored when n = 5");

	/* 7. Unsigned char comparison (values > 127) */
	check_cmp(ft_memcmp("\200", "\0", 1), 128, "ft_memcmp('\\200', '\\0', 1) is positive (unsigned char)");
	check_int(ft_memcmp("\200", "\0", 1), 128, "ft_memcmp('\\200', '\\0', 1) == 128");
	check_cmp(ft_memcmp("\0", "\200", 1), -128, "ft_memcmp('\\0', '\\200', 1) is negative (unsigned char)");
	check_int(ft_memcmp("\0", "\200", 1), -128, "ft_memcmp('\\0', '\\200', 1) == -128");
	check_cmp(ft_memcmp("\xff", "\x01", 1), 254, "ft_memcmp('\\xff', '\\x01', 1) == 254");
	check_int(ft_memcmp("\xff", "\x01", 1), 254, "ft_memcmp exact difference 255 - 1");
	check_cmp(ft_memcmp("\x01", "\xff", 1), -254, "ft_memcmp('\\x01', '\\xff', 1) == -254");
	check_int(ft_memcmp("\x01", "\xff", 1), -254, "ft_memcmp exact difference 1 - 255");
	check_cmp(ft_memcmp("\xaa\xbb\xcc", "\xaa\xbb\xcd", 3), memcmp("\xaa\xbb\xcc", "\xaa\xbb\xcd", 3),
		"ft_memcmp compares binary values with high bits");

	/* 8. Raw binary / struct comparison */
	check_int(ft_memcmp(arr1, arr2, sizeof(arr1)), 0, "ft_memcmp matches identical int arrays");
	check_cmp(ft_memcmp(arr1, arr3, sizeof(arr1)), memcmp(arr1, arr3, sizeof(arr1)),
		"ft_memcmp detects difference in int array");

	/* 9. Crash tests */
	check_crash(crash_memcmp_null_s1, "ft_memcmp(NULL, \"abc\", 3) raises SIGSEGV (crash attendu)");
	check_crash(crash_memcmp_null_s2, "ft_memcmp(\"abc\", NULL, 3) raises SIGSEGV (crash attendu)");
}

/* ========================================================================== */
/*                              ft_atoi                                       */
/* ========================================================================== */

