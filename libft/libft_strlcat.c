#include "libft_tests.h"

static void crash_strlcat_null_src(void)
{
	char dst[20] = "hello";
	ft_strlcat(dst, NULL, sizeof(dst));
}

static void crash_strlcat_null_dst(void)
{
	ft_strlcat(NULL, "hello", 10);
}

void test_strlcat(void)
{
	char	dst[32];
	size_t	ret;

	printf("\n--- Testing ft_strlcat ---\n");

	/* 1. Basic concatenation with plenty of room */
	memset(dst, 'Z', sizeof(dst));
	strcpy(dst, "Hello, ");
	ret = ft_strlcat(dst, "world!", sizeof(dst));
	check_uint(ret, 13, "ft_strlcat basic returns strlen(dst) + strlen(src)");
	check_str(dst, "Hello, world!", "ft_strlcat basic concatenates strings accurately");
	check(dst[13] == '\0', "ft_strlcat null-terminates result");
	check(dst[14] == 'Z', "ft_strlcat preserves buffer after null terminator");

	/* 2. Exact fit (size == strlen(dst) + strlen(src) + 1) */
	memset(dst, 'Z', sizeof(dst));
	strcpy(dst, "Hello");
	ret = ft_strlcat(dst, "!", 7);
	check_uint(ret, 6, "ft_strlcat exact fit returns 6");
	check_str(dst, "Hello!", "ft_strlcat exact fit copies char and null-terminates");
	check(dst[6] == '\0', "ft_strlcat exact fit has null byte at dst[6]");
	check(dst[7] == 'Z', "ft_strlcat exact fit preserves guard byte dst[7]");

	/* 3. Truncation: size allows some chars from src */
	memset(dst, 'Z', sizeof(dst));
	strcpy(dst, "Hello");
	ret = ft_strlcat(dst, ", world!", 9);
	check_uint(ret, 13, "ft_strlcat truncated returns total initial dst_len + src_len");
	check_str(dst, "Hello, w", "ft_strlcat truncated copies size - dst_len - 1 chars");
	check(dst[8] == '\0', "ft_strlcat truncated null-terminates at dst[size - 1]");
	check(dst[9] == 'Z', "ft_strlcat truncated preserves guard byte at dst[size]");

	/* 4. Truncation: size == strlen(dst) + 1 (room only for null terminator, 0 chars from src) */
	memset(dst, 'Z', sizeof(dst));
	strcpy(dst, "Hello");
	ret = ft_strlcat(dst, " world", 6);
	check_uint(ret, 11, "ft_strlcat (size == dst_len + 1) returns dst_len + src_len");
	check_str(dst, "Hello", "ft_strlcat (size == dst_len + 1) does not append src chars");
	check(dst[5] == '\0', "ft_strlcat (size == dst_len + 1) null-terminates at dst[5]");
	check(dst[6] == 'Z', "ft_strlcat (size == dst_len + 1) preserves guard byte");

	/* 5. Edge case: size == strlen(dst) (MUST NOT append and MUST NOT NUL-terminate) */
	memset(dst, 'Z', sizeof(dst));
	strcpy(dst, "Hello");
	ret = ft_strlcat(dst, " world", 5);
	check_uint(ret, 11, "ft_strlcat (size == dst_len) returns size + src_len");
	check(memcmp(dst, "Hello", 5) == 0, "ft_strlcat (size == dst_len) leaves dst untouched");
	check(dst[5] == '\0', "ft_strlcat (size == dst_len) preserves original null terminator");

	/* 6. Edge case: size < strlen(dst) */
	memset(dst, 'Z', sizeof(dst));
	strcpy(dst, "Hello");
	ret = ft_strlcat(dst, " world", 2);
	check_uint(ret, 8, "ft_strlcat (size < dst_len) returns size + src_len");
	check(memcmp(dst, "Hello", 5) == 0, "ft_strlcat (size < dst_len) leaves dst completely untouched");

	/* 7. Edge case: size == 0 */
	memset(dst, 'Z', sizeof(dst));
	strcpy(dst, "Hello");
	ret = ft_strlcat(dst, " world", 0);
	check_uint(ret, 6, "ft_strlcat (size == 0) returns size + src_len (0 + 6)");
	check_str(dst, "Hello", "ft_strlcat (size == 0) leaves dst untouched");

	/* 8. Empty dst with size > 0 */
	memset(dst, 'Z', sizeof(dst));
	dst[0] = '\0';
	ret = ft_strlcat(dst, "Hello", sizeof(dst));
	check_uint(ret, 5, "ft_strlcat on empty dst returns strlen(src)");
	check_str(dst, "Hello", "ft_strlcat on empty dst copies entire src");
	check(dst[5] == '\0', "ft_strlcat on empty dst null-terminates");
	check(dst[6] == 'Z', "ft_strlcat on empty dst preserves buffer after null");

	/* 9. Empty dst with size == 0 */
	memset(dst, 'Z', sizeof(dst));
	dst[0] = '\0';
	ret = ft_strlcat(dst, "Hello", 0);
	check_uint(ret, 5, "ft_strlcat on empty dst with size = 0 returns strlen(src)");
	check(dst[0] == '\0' && dst[1] == 'Z', "ft_strlcat on empty dst with size = 0 touches nothing");

	/* 10. Empty src */
	memset(dst, 'Z', sizeof(dst));
	strcpy(dst, "Hello");
	ret = ft_strlcat(dst, "", sizeof(dst));
	check_uint(ret, 5, "ft_strlcat with empty src returns strlen(dst)");
	check_str(dst, "Hello", "ft_strlcat with empty src leaves dst unchanged");

	/* 11. Empty dst and empty src */
	memset(dst, 'Z', sizeof(dst));
	dst[0] = '\0';
	ret = ft_strlcat(dst, "", sizeof(dst));
	check_uint(ret, 0, "ft_strlcat empty dst and empty src returns 0");
	check_str(dst, "", "ft_strlcat empty dst and empty src produces empty string");

	/* 12. BSD feature: dst is NULL when size is 0 */
	ret = ft_strlcat(NULL, "Hello", 0);
	check_uint(ret, 5, "ft_strlcat(NULL, src, 0) returns strlen(src) safely without crash");

	/* 13. Crash tests */
	check_crash(crash_strlcat_null_src, "ft_strlcat(dst, NULL, 10) raises SIGSEGV (crash attendu)");
	check_crash(crash_strlcat_null_dst, "ft_strlcat(NULL, src, 10) raises SIGSEGV (crash attendu)");
}

/* ========================================================================== */
/*                             ft_strncmp                                     */
/* ========================================================================== */

