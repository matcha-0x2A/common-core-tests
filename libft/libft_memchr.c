#include "libft_tests.h"

static void crash_memchr_null(void)
{
	ft_memchr(NULL, 'A', 5);
}

void test_memchr(void)
{
	char			str[] = "Hello, 42 World!";
	char			bin[] = {'b', 'o', 'n', '\0', 'j', 'o', 'u', 'r'};
	unsigned char	high[] = {0, 42, 128, 255, 127, 170};
	char			big[1024];

	printf("\n--- Testing ft_memchr ---\n");

	/* 1. Basic search in string */
	check(ft_memchr(str, 'H', 16) == str, "ft_memchr(str, 'H', 16) == str (first char)");
	check(ft_memchr(str, '4', 16) == str + 7, "ft_memchr(str, '4', 16) == str + 7 (middle char)");
	check(ft_memchr(str, '!', 16) == str + 15, "ft_memchr(str, '!', 16) == str + 15 (last char)");
	check(ft_memchr(str, 'o', 16) == str + 4, "ft_memchr(str, 'o', 16) == str + 4 (first occurrence of 'o')");

	/* 2. Character not found or outside search range */
	check(ft_memchr(str, 'z', 16) == NULL, "ft_memchr(str, 'z', 16) == NULL (not found)");
	check(ft_memchr(str, 'W', 10) == NULL, "ft_memchr(str, 'W', 10) == NULL ('W' is at index 10, n=10)");
	check(ft_memchr(str, 'W', 11) == str + 10, "ft_memchr(str, 'W', 11) == str + 10 ('W' is at index 10, n=11)");
	check(ft_memchr(str, 'o', 4) == NULL, "ft_memchr(str, 'o', 4) == NULL ('o' is at index 4, n=4)");
	check(ft_memchr(str, 'o', 5) == str + 4, "ft_memchr(str, 'o', 5) == str + 4 ('o' is at index 4, n=5)");

	/* 3. n == 0 (must always return NULL) */
	check(ft_memchr(str, 'H', 0) == NULL, "ft_memchr(str, 'H', 0) == NULL (n=0 with matching char at 0)");
	check(ft_memchr("", 'a', 0) == NULL, "ft_memchr(\"\", 'a', 0) == NULL (empty string, n=0)");
	check(ft_memchr("", '\0', 0) == NULL, "ft_memchr(\"\", '\\0', 0) == NULL (empty string, search '\\0', n=0)");

	/* 4. Searching for NUL byte ('\0') */
	check(ft_memchr(str, '\0', 16) == NULL, "ft_memchr(str, '\\0', 16) == NULL (before NUL terminator)");
	check(ft_memchr(str, '\0', 17) == str + 16, "ft_memchr(str, '\\0', 17) == str + 16 (matches NUL terminator)");

	/* 5. Binary data containing '\0' (memchr must NOT stop at '\0') */
	check(ft_memchr(bin, 'j', 8) == bin + 4, "ft_memchr(bin, 'j', 8) == bin + 4 (search past embedded '\\0')");
	check(ft_memchr(bin, 'r', 8) == bin + 7, "ft_memchr(bin, 'r', 8) == bin + 7 (last byte past embedded '\\0')");
	check(ft_memchr(bin, '\0', 8) == bin + 3, "ft_memchr(bin, '\\0', 8) == bin + 3 (search embedded '\\0')");
	check(ft_memchr(bin, 'x', 8) == NULL, "ft_memchr(bin, 'x', 8) == NULL (not found in binary)");

	/* 6. Unsigned char conversions (> 127, negative values, c > 255) */
	check(ft_memchr(high, 128, 6) == high + 2, "ft_memchr(high, 128, 6) == high + 2 (search 128 / 0x80)");
	check(ft_memchr(high, -128, 6) == high + 2, "ft_memchr(high, -128, 6) == high + 2 (search -128 converted to uchar)");
	check(ft_memchr(high, 255, 6) == high + 3, "ft_memchr(high, 255, 6) == high + 3 (search 255 / 0xFF)");
	check(ft_memchr(high, -1, 6) == high + 3, "ft_memchr(high, -1, 6) == high + 3 (search -1 converted to uchar)");
	check(ft_memchr(str, 'W' + 256, 16) == str + 10, "ft_memchr(str, 'W' + 256, 16) == str + 10 (c > 255 truncated to uchar)");
	check(ft_memchr(str, 'H' + 512, 16) == str, "ft_memchr(str, 'H' + 512, 16) == str (c > 255 truncated to uchar)");

	/* 7. Direct comparison against libc memchr */
	check(ft_memchr(str, 'W', 16) == memchr(str, 'W', 16), "ft_memchr matches libc memchr (found)");
	check(ft_memchr(str, 'z', 16) == memchr(str, 'z', 16), "ft_memchr matches libc memchr (not found)");
	check(ft_memchr(bin, 'j', 8) == memchr(bin, 'j', 8), "ft_memchr matches libc memchr (past embedded '\\0')");
	check(ft_memchr(high, 170, 6) == memchr(high, 170, 6), "ft_memchr matches libc memchr (high byte)");

	/* 8. Larger memory area */
	memset(big, 'A', sizeof(big));
	big[1000] = 'Z';
	check(ft_memchr(big, 'Z', sizeof(big)) == big + 1000, "ft_memchr(big, 'Z', 1024) == big + 1000");
	check(ft_memchr(big, 'Z', 1000) == NULL, "ft_memchr(big, 'Z', 1000) == NULL (searched 1000, target at 1000)");

	/* 9. NULL pointer handling (crash attendu si n > 0, pas de crash si n = 0) */
	check(ft_memchr(NULL, 'A', 0) == NULL, "ft_memchr(NULL, 'A', 0) == NULL (n = 0, no crash)");
	check_crash(crash_memchr_null, "ft_memchr(NULL, 'A', 5) raises SIGSEGV (crash attendu)");
}

