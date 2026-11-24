#include "libft_tests.h"

static char	map_toupper(unsigned int i, char c)
{
	(void)i;
	if (c >= 'a' && c <= 'z')
		return (c - 32);
	return (c);
}

static char	map_add_index(unsigned int i, char c)
{
	return (c + i);
}

static char	map_alternating(unsigned int i, char c)
{
	if (i % 2 == 0)
	{
		if (c >= 'a' && c <= 'z')
			return (c - 32);
	}
	else
	{
		if (c >= 'A' && c <= 'Z')
			return (c + 32);
	}
	return (c);
}

static char	map_only_index(unsigned int i, char c)
{
	(void)c;
	return ('0' + (i % 10));
}

static void	crash_strmapi_null_s(void)
{
	ft_strmapi(NULL, map_toupper);
}

static void	crash_strmapi_null_f(void)
{
	ft_strmapi("hello", NULL);
}

static void	crash_strmapi_all_null(void)
{
	ft_strmapi(NULL, NULL);
}

static void *call_strmapi_malloc(void)
{
	return (ft_strmapi("hello world", map_toupper));
}

void test_strmapi(void)
{
	char		*res;
	const char	*src = "abcdef";

	printf("\n--- Testing ft_strmapi ---\n");

	/* 1. Basic transformation (toupper) */
	res = ft_strmapi("hello world", map_toupper);
	check(res != NULL, "ft_strmapi(\"hello world\", map_toupper) returns non-NULL");
	if (res)
	{
		check_str(res, "HELLO WORLD", "Applies transformation to each char (\"HELLO WORLD\")");
		check_uint(strlen(res), 11, "Length matches expected (11)");
		free(res);
	}

	/* 2. Transformation using index */
	res = ft_strmapi("aaaaa", map_add_index);
	check(res != NULL, "ft_strmapi with index function returns non-NULL");
	if (res)
	{
		check_str(res, "abcde", "Correctly passes index (i = 0..4: 'a'+i -> \"abcde\")");
		free(res);
	}

	/* 3. Alternating case based on even/odd index */
	res = ft_strmapi("hello world", map_alternating);
	check(res != NULL, "ft_strmapi with alternating index returns non-NULL");
	if (res)
	{
		check_str(res, "HeLlO WoRlD", "Even indices uppercase, odd indices lowercase (\"HeLlO WoRlD\")");
		free(res);
	}

	/* 4. Large index mapping */
	res = ft_strmapi("xxxxxxxxxxxx", map_only_index);
	check(res != NULL, "ft_strmapi with index sequence returns non-NULL");
	if (res)
	{
		check_str(res, "012345678901", "Indices wrap correctly (012345678901)");
		free(res);
	}

	/* 5. Memory independence & immutability */
	res = ft_strmapi(src, map_toupper);
	check(res != NULL, "ft_strmapi returns non-NULL on src");
	if (res)
	{
		check(res != src, "Allocates new memory distinct from original string");
		check_str(src, "abcdef", "Original string remains unmodified");
		check_str(res, "ABCDEF", "Result string is transformed (\"ABCDEF\")");
		free(res);
	}

	/* 6. Empty string */
	res = ft_strmapi("", map_toupper);
	check(res != NULL, "ft_strmapi(\"\", f) returns non-NULL");
	if (res)
	{
		check_str(res, "", "Empty string returns allocated empty string (\"\")");
		check_uint(strlen(res), 0, "Length of empty string result is 0");
		free(res);
	}

	/* 7. Single character string */
	res = ft_strmapi("z", map_toupper);
	check(res != NULL, "ft_strmapi(\"z\", f) returns non-NULL");
	if (res)
	{
		check_str(res, "Z", "Single character string mapped correctly (\"Z\")");
		free(res);
	}

	/* 8. Defensive NULL checks */
	check_no_crash(crash_strmapi_null_s, "ft_strmapi(NULL, f) handles NULL string safely (no crash)");
	check(ft_strmapi(NULL, map_toupper) == NULL, "ft_strmapi(NULL, f) returns NULL");
	check_no_crash(crash_strmapi_null_f, "ft_strmapi(s, NULL) handles NULL function safely (no crash)");
	check(ft_strmapi("hello", NULL) == NULL, "ft_strmapi(s, NULL) returns NULL");
	check_no_crash(crash_strmapi_all_null, "ft_strmapi(NULL, NULL) handles all NULL safely (no crash)");
	check(ft_strmapi(NULL, NULL) == NULL, "ft_strmapi(NULL, NULL) returns NULL");

	/* 9. Malloc failure check */
	check_malloc_protection(call_strmapi_malloc, "ft_strmapi returns NULL and leaves 0 leaks on malloc failure");
}
