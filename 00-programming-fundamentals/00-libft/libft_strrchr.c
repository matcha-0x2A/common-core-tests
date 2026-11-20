#include "libft_tests.h"

static void crash_strrchr_null(void)
{
	ft_strrchr(NULL, 'a');
}

void test_strrchr(void)
{
	const char	*str = "Hello, 42 World!";
	const char	*empty = "";
	char		ext[] = {'a', (char)128, 'b', (char)128, 'c', '\0'};

	printf("\n--- Testing ft_strrchr ---\n");

	/* 1. Multiple occurrences: MUST return LAST occurrence */
	check(ft_strrchr(str, 'l') == str + 13, "ft_strrchr returns LAST occurrence of 'l' (index 13)");
	check(ft_strrchr(str, 'o') == str + 11, "ft_strrchr returns LAST occurrence of 'o' (index 11)");

	/* 2. Single occurrence: start, middle, end */
	check(ft_strrchr(str, 'H') == str, "ft_strrchr finds single character at start");
	check(ft_strrchr(str, '4') == str + 7, "ft_strrchr finds single character in middle");
	check(ft_strrchr(str, '!') == str + 15, "ft_strrchr finds single character at end");

	/* 3. Not found */
	check(ft_strrchr(str, 'z') == NULL, "ft_strrchr returns NULL when char not found");
	check(ft_strrchr(empty, 'a') == NULL, "ft_strrchr(\"\", 'a') returns NULL");

	/* 4. Terminating null byte '\\0' search */
	check(ft_strrchr(str, '\0') == str + 16, "ft_strrchr finds terminating '\\0'");
	check(ft_strrchr(empty, '\0') == empty, "ft_strrchr(\"\", '\\0') finds terminating '\\0'");

	/* 5. Int conversion: c > 255 truncated to char */
	check(ft_strrchr(str, 'l' + 256) == str + 13, "ft_strrchr with c > 255 converts to char (last 'l')");
	check(ft_strrchr(str, 'o' + 512) == str + 11, "ft_strrchr with c + 512 converts to char (last 'o')");

	/* 6. Extended ASCII / signed char */
	check(ft_strrchr(ext, 128) == ext + 3, "ft_strrchr finds last extended ASCII char (128)");
	check(ft_strrchr(ext, -128) == ext + 3, "ft_strrchr handles signed char negative value");

	/* 7. Comparison with libc strrchr */
	check(ft_strrchr(str, 'l') == strrchr(str, 'l'), "ft_strrchr matches libc strrchr (multiple 'l')");
	check(ft_strrchr(str, 'o') == strrchr(str, 'o'), "ft_strrchr matches libc strrchr (multiple 'o')");
	check(ft_strrchr(str, 'X') == strrchr(str, 'X'), "ft_strrchr matches libc strrchr (not found)");
	check(ft_strrchr(str, '\0') == strrchr(str, '\0'), "ft_strrchr matches libc strrchr on '\\0'");

	/* 8. Crash on NULL */
	check_crash(crash_strrchr_null, "ft_strrchr(NULL, 'a') raises SIGSEGV (crash attendu)");
}

