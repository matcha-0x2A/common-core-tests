#include "libft_tests.h"

static void	crash_strnstr_null_big(void)
{
	ft_strnstr(NULL, "needle", 5);
}

static void	crash_strnstr_null_little(void)
{
	ft_strnstr("haystack", NULL, 5);
}

static void	safe_strnstr_null_big_len0(void)
{
	char *res = ft_strnstr(NULL, "needle", 0);
	if (res != NULL)
		exit(1);
}

void	test_strnstr(void)
{
	const char	*big = "Hello 42 School, welcome to Libft!";
	const char	*empty = "";

	printf("\n--- Testing ft_strnstr ---\n");

	/* 1. Basic occurrences: start, middle, end */
	check(ft_strnstr(big, "Hello", 34) == big,
		"ft_strnstr finds needle at start of haystack");
	check(ft_strnstr(big, "42 School", 34) == big + 6,
		"ft_strnstr finds needle in the middle");
	check(ft_strnstr(big, "Libft!", 34) == big + 28,
		"ft_strnstr finds needle at the end of haystack");

	/* 2. Empty needle: must return big immediately */
	check(ft_strnstr(big, "", 34) == big,
		"ft_strnstr with empty needle returns big");
	check(ft_strnstr(big, "", 0) == big,
		"ft_strnstr with empty needle and len == 0 returns big");
	check(ft_strnstr(empty, "", 0) == empty,
		"ft_strnstr with empty big and empty needle returns big");
	check(ft_strnstr(empty, "42", 5) == NULL,
		"ft_strnstr with empty big and non-empty needle returns NULL");

	/* 3. Needle not found */
	check(ft_strnstr(big, "NotFound", 34) == NULL,
		"ft_strnstr returns NULL when needle is absent");
	check(ft_strnstr(big, "Libft?", 34) == NULL,
		"ft_strnstr returns NULL on partial mismatch");

	/* 4. Length limitations (len parameter boundaries) */
	check(ft_strnstr(big, "Hello", 4) == NULL,
		"ft_strnstr returns NULL when len < needle length (len=4 < 5)");
	check(ft_strnstr(big, "Hello", 5) == big,
		"ft_strnstr finds needle when len == needle length");
	check(ft_strnstr(big, "42 School", 14) == NULL,
		"ft_strnstr returns NULL when match ends at or beyond len (len=14, needs 15)");
	check(ft_strnstr(big, "42 School", 15) == big + 6,
		"ft_strnstr finds needle when len exactly covers it (len=15)");
	check(ft_strnstr(big, "Hello", 0) == NULL,
		"ft_strnstr with len == 0 and non-empty needle returns NULL");

	/* 5. Needle longer than haystack */
	check(ft_strnstr("42", "42 School", 10) == NULL,
		"ft_strnstr returns NULL when needle is longer than haystack");

	/* 6. Repetitive patterns & backtracking */
	const char *s_repeat = "aaabaaa";
	const char *s_backtrack = "aababac";
	const char *s_similar = "fake fact";
	check(ft_strnstr(s_repeat, "aaba", 7) == s_repeat + 1,
		"ft_strnstr finds needle with repeating prefix (\"aaabaaa\", \"aaba\")");
	check(ft_strnstr(s_backtrack, "abac", 7) == s_backtrack + 3,
		"ft_strnstr correctly skips false prefixes (\"aababac\", \"abac\")");
	check(ft_strnstr(s_similar, "fact", 9) == s_similar + 5,
		"ft_strnstr finds correct word after similar prefix (\"fake fact\", \"fact\")");

	/* 7. Exact length strings */
	const char *s_exact = "abcdef";
	check(ft_strnstr(s_exact, "abcdef", 6) == s_exact,
		"ft_strnstr finds full string match with exact len");
	check(ft_strnstr(s_exact, "abcdef", 5) == NULL,
		"ft_strnstr returns NULL when exact match truncated by len=5");

	/* 8. NULL handling / Segfault protection */
	check_no_crash(safe_strnstr_null_big_len0,
		"ft_strnstr(NULL, \"needle\", 0) does not crash and returns NULL");
	check_crash(crash_strnstr_null_big,
		"ft_strnstr(NULL, \"needle\", 5) raises SIGSEGV (crash attendu)");
	check_crash(crash_strnstr_null_little,
		"ft_strnstr(\"haystack\", NULL, 5) raises SIGSEGV (crash attendu)");
}
