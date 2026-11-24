#include "libft_tests.h"

static void crash_strtrim_null_s1(void)
{
	ft_strtrim(NULL, " \t");
}

static void crash_strtrim_null_set(void)
{
	ft_strtrim("hello", NULL);
}

static void crash_strtrim_all_null(void)
{
	ft_strtrim(NULL, NULL);
}

static void *call_strtrim_malloc(void)
{
	return (ft_strtrim("   hello world   ", " "));
}

void test_strtrim(void)
{
	char	*res;

	printf("\n--- Testing ft_strtrim ---\n");

	/* 1. Basic trim spaces on both sides */
	res = ft_strtrim("   hello world   ", " ");
	check(res != NULL, "ft_strtrim(\"   hello world   \", \" \") returns non-NULL");
	if (res)
	{
		check_str(res, "hello world", "Trims leading and trailing spaces (\"hello world\")");
		check_uint(strlen(res), 11, "Length is 11");
		free(res);
	}

	/* 2. Multiple characters in set in varied order */
	res = ft_strtrim("abcbcaHELLOcbacba", "abc");
	check(res != NULL, "ft_strtrim returns non-NULL with multi-char set");
	if (res)
	{
		check_str(res, "HELLO", "Trims all set chars regardless of order (\"HELLO\")");
		free(res);
	}

	/* 3. Trim only at the beginning */
	res = ft_strtrim("---hello", "-");
	check(res != NULL, "ft_strtrim returns non-NULL for prefix trim");
	if (res)
	{
		check_str(res, "hello", "Trims only prefix (\"---hello\" -> \"hello\")");
		free(res);
	}

	/* 4. Trim only at the end */
	res = ft_strtrim("hello---", "-");
	check(res != NULL, "ft_strtrim returns non-NULL for suffix trim");
	if (res)
	{
		check_str(res, "hello", "Trims only suffix (\"hello---\" -> \"hello\")");
		free(res);
	}

	/* 5. Set characters in the middle are preserved */
	res = ft_strtrim("---hello---world---", "-");
	check(res != NULL, "ft_strtrim preserves inner matching characters");
	if (res)
	{
		check_str(res, "hello---world", "Inner set characters remain untouched (\"hello---world\")");
		free(res);
	}

	/* 6. No characters to trim (set not found at ends) */
	{
		const char *s1 = "hello";
		res = ft_strtrim(s1, "x");
		check(res != NULL, "ft_strtrim returns non-NULL when no characters trimmed");
		if (res)
		{
			check(res != s1, "Allocates new distinct memory");
			check_str(res, "hello", "String remains unchanged when set chars absent");
			free(res);
		}
	}

	/* 7. Empty set */
	res = ft_strtrim("hello", "");
	check(res != NULL, "ft_strtrim with empty set returns non-NULL");
	if (res)
	{
		check_str(res, "hello", "Empty set leaves string unchanged (\"hello\")");
		free(res);
	}

	/* 8. Entire string consists of set characters (All trimmed away!) */
	res = ft_strtrim("     ", " ");
	check(res != NULL, "ft_strtrim returns non-NULL when entire string trimmed");
	if (res)
	{
		check_str(res, "", "All spaces trimmed returns empty string (\"\")");
		check_uint(strlen(res), 0, "Length is 0");
		free(res);
	}

	res = ft_strtrim("abcba", "abc");
	check(res != NULL, "ft_strtrim on \"abcba\" with set \"abc\" returns non-NULL");
	if (res)
	{
		check_str(res, "", "All set chars trimmed returns empty string (\"\")");
		free(res);
	}

	/* 9. Empty s1 */
	res = ft_strtrim("", "abc");
	check(res != NULL, "ft_strtrim(\"\", \"abc\") returns non-NULL");
	if (res)
	{
		check_str(res, "", "Empty s1 returns empty string");
		free(res);
	}

	res = ft_strtrim("", "");
	check(res != NULL, "ft_strtrim(\"\", \"\") returns non-NULL");
	if (res)
	{
		check_str(res, "", "Both empty returns empty string");
		free(res);
	}

	/* 10. Single character string */
	res = ft_strtrim("a", "a");
	check(res != NULL, "ft_strtrim(\"a\", \"a\") returns non-NULL");
	if (res)
	{
		check_str(res, "", "Single char matching set trimmed to empty string");
		free(res);
	}

	res = ft_strtrim("a", "b");
	check(res != NULL, "ft_strtrim(\"a\", \"b\") returns non-NULL");
	if (res)
	{
		check_str(res, "a", "Single char not in set preserved (\"a\")");
		free(res);
	}

	/* 11. Defensive NULL checks */
	check_no_crash(crash_strtrim_null_s1, "ft_strtrim(NULL, set) handles NULL s1 safely (no crash)");
	check_no_crash(crash_strtrim_null_set, "ft_strtrim(s1, NULL) handles NULL set safely (no crash)");
	check_no_crash(crash_strtrim_all_null, "ft_strtrim(NULL, NULL) handles all NULL safely (no crash)");

	/* 12. Malloc failure check */
	check_malloc_protection(call_strtrim_malloc, "ft_strtrim returns NULL and leaves 0 leaks on malloc failure");
}
