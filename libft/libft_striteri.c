#include "libft_tests.h"

static void	iter_toupper(unsigned int i, char *c)
{
	(void)i;
	if (*c >= 'a' && *c <= 'z')
		*c -= 32;
}

static void	iter_add_index(unsigned int i, char *c)
{
	*c = (char)(*c + i);
}

static void	iter_alternating(unsigned int i, char *c)
{
	if (i % 2 == 0)
	{
		if (*c >= 'a' && *c <= 'z')
			*c -= 32;
	}
	else
	{
		if (*c >= 'A' && *c <= 'Z')
			*c += 32;
	}
}

static void	iter_write_index(unsigned int i, char *c)
{
	*c = (char)('0' + (i % 10));
}

static void	crash_striteri_null_s(void)
{
	ft_striteri(NULL, iter_toupper);
}

static void	crash_striteri_null_f(void)
{
	char	str[] = "hello";

	ft_striteri(str, NULL);
}

static void	crash_striteri_all_null(void)
{
	ft_striteri(NULL, NULL);
}

void test_striteri(void)
{
	char	buf[64];

	printf("\n--- Testing ft_striteri ---\n");

	/* 1. Basic in-place modification (toupper) */
	strcpy(buf, "hello world");
	ft_striteri(buf, iter_toupper);
	check_str(buf, "HELLO WORLD", "Modifies string in-place to uppercase (\"HELLO WORLD\")");

	/* 2. In-place modification using index */
	strcpy(buf, "aaaaa");
	ft_striteri(buf, iter_add_index);
	check_str(buf, "abcde", "Correctly applies index in-place ('a'+i -> \"abcde\")");

	/* 3. Alternating modification based on even/odd index */
	strcpy(buf, "hello world");
	ft_striteri(buf, iter_alternating);
	check_str(buf, "HeLlO WoRlD", "Alternates even uppercase / odd lowercase in-place (\"HeLlO WoRlD\")");

	/* 4. Sequence of indices */
	strcpy(buf, "xxxxxxxxxxxx");
	ft_striteri(buf, iter_write_index);
	check_str(buf, "012345678901", "Indices wrap correctly in-place (012345678901)");

	/* 5. Empty string */
	strcpy(buf, "");
	ft_striteri(buf, iter_toupper);
	check_str(buf, "", "Empty string remains empty string");

	/* 6. Single character string */
	strcpy(buf, "z");
	ft_striteri(buf, iter_toupper);
	check_str(buf, "Z", "Single char modified in-place (\"Z\")");

	/* 7. Defensive NULL checks */
	check_no_crash(crash_striteri_null_s, "ft_striteri(NULL, f) handles NULL string safely (no crash)");
	check_no_crash(crash_striteri_null_f, "ft_striteri(s, NULL) handles NULL function safely (no crash)");
	check_no_crash(crash_striteri_all_null, "ft_striteri(NULL, NULL) handles all NULL safely (no crash)");

	/* 8. Verify string is untouched when f is NULL */
	strcpy(buf, "hello");
	ft_striteri(buf, NULL);
	check_str(buf, "hello", "String remains untouched when f is NULL");
}
