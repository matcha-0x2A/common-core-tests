#include "libft_tests.h"

static void crash_strchr_null(void)
{
	ft_strchr(NULL, 'a');
}

void test_strchr(void)
{
	const char	*str = "Hello, 42 World!";
	const char	*empty = "";
	char		ext[] = {'a', (char)128, 'b', '\0'};

	printf("\n--- Testing ft_strchr ---\n");

	/* 1. Basic matches: start, middle, end */
	check(ft_strchr(str, 'H') == str, "ft_strchr finds first character (start)");
	check(ft_strchr(str, '4') == str + 7, "ft_strchr finds middle character");
	check(ft_strchr(str, '!') == str + 15, "ft_strchr finds last character");

	/* 2. Multiple occurrences: must return first match */
	check(ft_strchr(str, 'o') == str + 4, "ft_strchr returns first occurrence of 'o'");
	check(ft_strchr(str, 'l') == str + 2, "ft_strchr returns first occurrence of 'l'");

	/* 3. Not found */
	check(ft_strchr(str, 'z') == NULL, "ft_strchr returns NULL when char not found");
	check(ft_strchr(empty, 'a') == NULL, "ft_strchr(\"\", 'a') returns NULL");

	/* 4. Terminating null byte '\\0' search */
	check(ft_strchr(str, '\0') == str + 16, "ft_strchr finds terminating '\\0'");
	check(ft_strchr(empty, '\0') == empty, "ft_strchr(\"\", '\\0') finds terminating '\\0'");

	/* 5. Int conversion: c > 255 truncated to char */
	check(ft_strchr(str, 'H' + 256) == str, "ft_strchr with c > 255 converts to char");
	check(ft_strchr(str, 'o' + 512) == str + 4, "ft_strchr with c + 512 converts to char");

	/* 6. Extended ASCII / signed char */
	check(ft_strchr(ext, 128) == ext + 1, "ft_strchr finds extended ASCII char (128)");
	check(ft_strchr(ext, -128) == ext + 1, "ft_strchr handles signed char negative value");

	/* 7. Comparison with libc strchr */
	check(ft_strchr(str, 'W') == strchr(str, 'W'), "ft_strchr matches libc strchr");
	check(ft_strchr(str, 'X') == strchr(str, 'X'), "ft_strchr matches libc strchr (not found)");
	check(ft_strchr(str, '\0') == strchr(str, '\0'), "ft_strchr matches libc strchr on '\\0'");

	/* 8. Crash on NULL */
	check_crash(crash_strchr_null, "ft_strchr(NULL, 'a') raises SIGSEGV (crash attendu)");
}

