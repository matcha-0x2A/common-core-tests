#include "libft_tests.h"

static void crash_strlcpy_null_src(void)
{
	char	dst[10];

	ft_strlcpy(dst, NULL, 5);
}

static void crash_strlcpy_null_dst(void)
{
	ft_strlcpy(NULL, "hello", 5);
}

void test_strlcpy(void)
{
	char		dst[32];
	const char	*src = "Hello, 42 World!";
	size_t		ret;

	printf("\n--- Testing ft_strlcpy ---\n");

	/* 1. Normal copy with plenty of room (size > strlen(src)) */
	memset(dst, 'Z', sizeof(dst));
	ret = ft_strlcpy(dst, src, sizeof(dst));
	check(ret == strlen(src), "ft_strlcpy returns strlen(src) on full copy");
	check(strcmp(dst, src) == 0, "ft_strlcpy copies full string accurately");
	check(dst[strlen(src)] == '\0', "ft_strlcpy null-terminates destination");
	check(dst[strlen(src) + 1] == 'Z', "ft_strlcpy does not write beyond null-terminator");

	/* 2. Exact fit (size == strlen(src) + 1) */
	memset(dst, 'Z', sizeof(dst));
	ret = ft_strlcpy(dst, "abc", 4);
	check(ret == 3, "ft_strlcpy exact fit returns 3");
	check(strcmp(dst, "abc") == 0, "ft_strlcpy exact fit copies all chars and null");
	check(dst[4] == 'Z', "ft_strlcpy exact fit preserves guard byte");

	/* 3. Truncation (size < strlen(src) + 1) */
	memset(dst, 'Z', sizeof(dst));
	ret = ft_strlcpy(dst, "hello world", 6);
	check(ret == 11, "ft_strlcpy returns strlen(src) even when truncated");
	check(strncmp(dst, "hello", 5) == 0, "ft_strlcpy copies size - 1 chars");
	check(dst[5] == '\0', "ft_strlcpy null-terminates truncated string at dst[size - 1]");
	check(dst[6] == 'Z', "ft_strlcpy preserves buffer past size");

	/* 4. size == 1 (only room for null-terminator) */
	memset(dst, 'Z', sizeof(dst));
	ret = ft_strlcpy(dst, "hello", 1);
	check(ret == 5, "ft_strlcpy with size = 1 returns strlen(src)");
	check(dst[0] == '\0', "ft_strlcpy with size = 1 sets dst[0] = '\\0'");
	check(dst[1] == 'Z', "ft_strlcpy with size = 1 leaves dst[1] untouched");

	/* 5. size == 0 (must NOT touch dst at all) */
	memset(dst, 'A', sizeof(dst));
	ret = ft_strlcpy(dst, "hello", 0);
	check(ret == 5, "ft_strlcpy with size = 0 returns strlen(src)");
	check(dst[0] == 'A', "ft_strlcpy with size = 0 does not modify dst");

	/* 6. Empty source string \"\" */
	memset(dst, 'Z', sizeof(dst));
	ret = ft_strlcpy(dst, "", sizeof(dst));
	check(ret == 0, "ft_strlcpy with empty src returns 0");
	check(dst[0] == '\0', "ft_strlcpy with empty src sets dst[0] = '\\0'");
	check(dst[1] == 'Z', "ft_strlcpy with empty src leaves dst[1] untouched");

	/* 7. Empty source string with size = 0 */
	memset(dst, 'B', sizeof(dst));
	ret = ft_strlcpy(dst, "", 0);
	check(ret == 0, "ft_strlcpy empty src and size = 0 returns 0");
	check(dst[0] == 'B', "ft_strlcpy empty src and size = 0 leaves dst untouched");

	/* 8. NULL handling */
	check_crash(crash_strlcpy_null_src, "ft_strlcpy(dst, NULL, 5) raises SIGSEGV (crash attendu)");
	check_crash(crash_strlcpy_null_dst, "ft_strlcpy(NULL, src, 5) raises SIGSEGV (crash attendu)");
}

/* ========================================================================== */
/*                             ft_strlcat                                     */
/* ========================================================================== */

