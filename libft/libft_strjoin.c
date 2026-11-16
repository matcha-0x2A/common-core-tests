#include "libft_tests.h"

static void crash_strjoin_null_s1(void)
{
	ft_strjoin(NULL, "world");
}

static void crash_strjoin_null_s2(void)
{
	ft_strjoin("hello", NULL);
}

static void crash_strjoin_all_null(void)
{
	ft_strjoin(NULL, NULL);
}

void test_strjoin(void)
{
	char	*res;
	char	long_s1[501];
	char	long_s2[501];
	int		i;

	printf("\n--- Testing ft_strjoin ---\n");

	/* 1. Basic concatenation */
	res = ft_strjoin("hello ", "world");
	check(res != NULL, "ft_strjoin(\"hello \", \"world\") returns non-NULL");
	if (res)
	{
		check_str(res, "hello world", "Concatenates basic strings (\"hello \" + \"world\")");
		check_uint(strlen(res), 11, "Length is 11");
		free(res);
	}

	/* 2. Concatenation with numbers and spaces */
	res = ft_strjoin("42 ", "Marseille");
	check(res != NULL, "ft_strjoin(\"42 \", \"Marseille\") returns non-NULL");
	if (res)
	{
		check_str(res, "42 Marseille", "Concatenates (\"42 \" + \"Marseille\")");
		free(res);
	}

	/* 3. Empty s1 */
	res = ft_strjoin("", "world");
	check(res != NULL, "ft_strjoin(\"\", \"world\") returns non-NULL");
	if (res)
	{
		check_str(res, "world", "Empty s1 returns s2 content (\"\" + \"world\")");
		check_uint(strlen(res), 5, "Length matches s2 length");
		free(res);
	}

	/* 4. Empty s2 */
	res = ft_strjoin("hello", "");
	check(res != NULL, "ft_strjoin(\"hello\", \"\") returns non-NULL");
	if (res)
	{
		check_str(res, "hello", "Empty s2 returns s1 content (\"hello\" + \"\")");
		check_uint(strlen(res), 5, "Length matches s1 length");
		free(res);
	}

	/* 5. Both empty */
	res = ft_strjoin("", "");
	check(res != NULL, "ft_strjoin(\"\", \"\") returns non-NULL");
	if (res)
	{
		check_str(res, "", "Both empty returns empty string (\"\")");
		check_uint(strlen(res), 0, "Length is 0");
		free(res);
	}

	/* 6. Single characters */
	res = ft_strjoin("a", "b");
	check(res != NULL, "ft_strjoin(\"a\", \"b\") returns non-NULL");
	if (res)
	{
		check_str(res, "ab", "Single characters (\"a\" + \"b\" -> \"ab\")");
		free(res);
	}

	/* 7. Memory independence (distinct pointers) */
	{
		const char *s1 = "first";
		const char *s2 = "second";
		res = ft_strjoin(s1, s2);
		check(res != NULL, "ft_strjoin returns allocated string");
		if (res)
		{
			check(res != s1 && res != s2, "Returned string is newly allocated (distinct pointers)");
			check_str(res, "firstsecond", "Content is accurate");
			free(res);
		}
	}

	/* 8. Long strings (1000 characters) */
	for (i = 0; i < 500; i++)
	{
		long_s1[i] = 'A';
		long_s2[i] = 'B';
	}
	long_s1[500] = '\0';
	long_s2[500] = '\0';
	res = ft_strjoin(long_s1, long_s2);
	check(res != NULL, "ft_strjoin handles two 500-char strings");
	if (res)
	{
		check_uint(strlen(res), 1000, "Total length is 1000");
		check(res[0] == 'A' && res[499] == 'A' && res[500] == 'B' && res[999] == 'B' && res[1000] == '\0',
			"Contents match 500 'A' followed by 500 'B' with null-terminator");
		free(res);
	}

	/* 9. Defensive NULL checks */
	check_no_crash(crash_strjoin_null_s1, "ft_strjoin(NULL, s2) handles NULL s1 safely (no crash)");
	check_no_crash(crash_strjoin_null_s2, "ft_strjoin(s1, NULL) handles NULL s2 safely (no crash)");
	check_no_crash(crash_strjoin_all_null, "ft_strjoin(NULL, NULL) handles all NULL safely (no crash)");
}
