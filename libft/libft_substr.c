#include "libft_tests.h"

static void crash_substr_null(void)
{
	ft_substr(NULL, 0, 5);
}

void test_substr(void)
{
	char		*sub;
	const char	*src = "Hello, 42 World!";
	const char	*empty = "";

	printf("\n--- Testing ft_substr ---\n");

	/* 1. Basic substring extraction in middle */
	sub = ft_substr(src, 7, 2);
	check(sub != NULL, "ft_substr(\"Hello, 42 World!\", 7, 2) returns non-NULL");
	if (sub)
	{
		check_str(sub, "42", "Extracted middle substring (\"42\")");
		check_uint(strlen(sub), 2, "Length is 2");
		free(sub);
	}

	/* 2. Substring extraction at the beginning (start = 0) */
	sub = ft_substr(src, 0, 5);
	check(sub != NULL, "ft_substr returns non-NULL at start = 0");
	if (sub)
	{
		check_str(sub, "Hello", "Extracted prefix (\"Hello\")");
		check(sub != src, "Allocates new memory (distinct pointer from src)");
		free(sub);
	}

	/* 3. Substring extraction at the very end */
	sub = ft_substr(src, 10, 6);
	check(sub != NULL, "ft_substr returns non-NULL at end");
	if (sub)
	{
		check_str(sub, "World!", "Extracted suffix (\"World!\")");
		free(sub);
	}

	/* 4. Substring with len = 1 (single character) */
	sub = ft_substr(src, 4, 1);
	check(sub != NULL, "ft_substr returns non-NULL for len = 1");
	if (sub)
	{
		check_str(sub, "o", "Extracted single char ('o')");
		check_uint(strlen(sub), 1, "Length is 1");
		free(sub);
	}

	/* 5. Substring with len = 0 -> must return allocated empty string \"\" */
	sub = ft_substr(src, 4, 0);
	check(sub != NULL, "ft_substr with len = 0 returns allocated empty string");
	if (sub)
	{
		check_str(sub, "", "Content is empty string \"\"");
		check_uint(strlen(sub), 0, "Length is 0");
		free(sub);
	}

	/* 6. start >= strlen(src) -> must return allocated empty string \"\" */
	sub = ft_substr(src, 50, 5);
	check(sub != NULL, "ft_substr with start > strlen returns allocated empty string");
	if (sub)
	{
		check_str(sub, "", "Content is empty string when start > strlen");
		free(sub);
	}

	sub = ft_substr(src, strlen(src), 5);
	check(sub != NULL, "ft_substr with start == strlen returns allocated empty string");
	if (sub)
	{
		check_str(sub, "", "Content is empty string when start == strlen");
		free(sub);
	}

	/* 7. len larger than remaining characters (start + len > strlen(src)) */
	sub = ft_substr(src, 10, 100);
	check(sub != NULL, "ft_substr with len > remaining characters returns non-NULL");
	if (sub)
	{
		check_str(sub, "World!", "Stops at end of string without overflow (\"World!\")");
		check_uint(strlen(sub), 6, "Length matches remaining chars (6)");
		free(sub);
	}

	/* 8. Full string extraction */
	sub = ft_substr(src, 0, strlen(src));
	check(sub != NULL, "ft_substr of full length returns non-NULL");
	if (sub)
	{
		check_str(sub, src, "Content matches entire original string");
		check(sub != src, "Allocates new memory distinct from original string");
		free(sub);
	}

	/* 9. Empty string source */
	sub = ft_substr(empty, 0, 5);
	check(sub != NULL, "ft_substr(\"\", 0, 5) returns allocated empty string");
	if (sub)
	{
		check_str(sub, "", "Empty source yields empty string");
		free(sub);
	}

	sub = ft_substr(empty, 5, 5);
	check(sub != NULL, "ft_substr(\"\", 5, 5) returns allocated empty string");
	if (sub)
	{
		check_str(sub, "", "Empty source with start > 0 yields empty string");
		free(sub);
	}

	/* 10. Large len with start = 0 */
	sub = ft_substr("42", 0, 42000);
	check(sub != NULL, "ft_substr(\"42\", 0, 42000) returns non-NULL");
	if (sub)
	{
		check_str(sub, "42", "Correctly handles giant len without reading out of bounds");
		free(sub);
	}

	/* 11. Defensive NULL checks */
	check_no_crash(crash_substr_null, "ft_substr(NULL, 0, 5) handles NULL safely (no crash)");
}
