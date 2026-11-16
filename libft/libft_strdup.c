#include "libft_tests.h"

static void crash_strdup_null(void)
{
	ft_strdup(NULL);
}

void test_strdup(void)
{
	char		*dup;
	const char	*empty = "";
	const char	*single = "a";
	const char	*str = "Hello, 42 World!";
	const char	*special = " \t\n\r\v\f~`!@#$%^&*()_+-={}|[]\\:\";'<>?,./";
	char		long_str[1001];
	char		orig_buf[32];
	size_t		i;

	printf("\n--- Testing ft_strdup ---\n");

	/* 1. Basic string duplication */
	dup = ft_strdup(str);
	check(dup != NULL, "ft_strdup(\"Hello, 42 World!\") returns non-NULL");
	if (dup)
	{
		check(dup != str, "ft_strdup allocates new memory (distinct pointer)");
		check(strcmp(dup, str) == 0, "ft_strdup duplicates string content accurately");
		check(strlen(dup) == strlen(str), "ft_strdup preserves exact length");
		free(dup);
	}

	/* 2. Empty string */
	dup = ft_strdup(empty);
	check(dup != NULL, "ft_strdup(\"\") returns non-NULL");
	if (dup)
	{
		check(dup != empty, "ft_strdup(\"\") allocates distinct pointer");
		check(strcmp(dup, "") == 0, "ft_strdup(\"\") duplicates empty string");
		check(dup[0] == '\0', "ft_strdup(\"\")[0] == '\\0'");
		free(dup);
	}

	/* 3. Single character string */
	dup = ft_strdup(single);
	check(dup != NULL, "ft_strdup(\"a\") returns non-NULL");
	if (dup)
	{
		check(strcmp(dup, "a") == 0, "ft_strdup(\"a\") duplicates single char");
		free(dup);
	}

	/* 4. Special and whitespace characters */
	dup = ft_strdup(special);
	check(dup != NULL, "ft_strdup(special_chars) returns non-NULL");
	if (dup)
	{
		check(strcmp(dup, special) == 0, "ft_strdup preserves special/whitespace chars");
		free(dup);
	}

	/* 5. Memory independence: modifying duplicate must not affect original */
	strcpy(orig_buf, "original");
	dup = ft_strdup(orig_buf);
	check(dup != NULL, "ft_strdup for independence check returns non-NULL");
	if (dup)
	{
		dup[0] = 'X';
		check(orig_buf[0] == 'o', "Modifying duplicate does not affect original");
		orig_buf[1] = 'Y';
		check(dup[1] == 'r', "Modifying original does not affect duplicate");
		free(dup);
	}

	/* 6. Long string (1000 characters) */
	for (i = 0; i < 1000; i++)
		long_str[i] = 'A' + (i % 26);
	long_str[1000] = '\0';
	dup = ft_strdup(long_str);
	check(dup != NULL, "ft_strdup(1000 chars) returns non-NULL");
	if (dup)
	{
		check(strcmp(dup, long_str) == 0, "ft_strdup accurately duplicates 1000 chars");
		free(dup);
	}

	/* 7. Crash check on NULL */
	check_crash(crash_strdup_null, "ft_strdup(NULL) raises SIGSEGV (crash attendu)");
}

