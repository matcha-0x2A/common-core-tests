#include "libft_tests.h"

static void crash_memcpy_null_dest(void)
{
	char	src[] = "hello";

	ft_memcpy(NULL, src, 5);
}

static void crash_memcpy_null_src(void)
{
	char	dest[10];

	ft_memcpy(dest, NULL, 5);
}

static void test_memcpy_null_null(void)
{
	pid_t	pid;
	int		status;

	g_tests_run++;
	fflush(stdout);
	pid = fork();
	if (pid < 0)
	{
		g_tests_failed++;
		printf("  [%sFAIL%s] ft_memcpy(NULL, NULL, 3) (fork failed)\n", RED, RESET);
		return ;
	}
	if (pid == 0)
	{
		void *res = ft_memcpy(NULL, NULL, 3);
		if (res == NULL)
			_exit(0);
		_exit(1);
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
		printf("  [%sOK%s] ft_memcpy(NULL, NULL, 3) == NULL (conforme 42)\n", GREEN, RESET);
	else if (WIFSIGNALED(status) && WTERMSIG(status) == SIGSEGV)
		printf("  [%sOK%s] ft_memcpy(NULL, NULL, 3) crashed with signal %d (conforme libc)\n", GREEN, RESET, WTERMSIG(status));
	else if (WIFSIGNALED(status))
	{
		g_tests_failed++;
		printf("  [%sFAIL%s] ft_memcpy(NULL, NULL, 3) crashed with signal %d (expected NULL ou SIGSEGV)\n", RED, RESET, WTERMSIG(status));
	}
	else
	{
		g_tests_failed++;
		printf("  [%sFAIL%s] ft_memcpy(NULL, NULL, 3) returned non-NULL\n", RED, RESET);
	}
}

void test_memcpy(void)
{
	char	src[] = "Hello, 42 World!";
	char	dest1[32];
	char	dest2[32];
	char	guard_buf[20];
	char	bin_src[] = {'4', '2', '\0', 'T', 'o', 'k', 'y', 'o', '\0', '!'};
	char	bin_dest[16];
	char	large_src[1024];
	char	large_dest[1024];
	size_t	i;

	printf("\n--- Testing ft_memcpy ---\n");

	/* 1. Return value: must return dest pointer */
	check(ft_memcpy(dest1, src, 16) == dest1, "ft_memcpy returns dest pointer");

	/* 2. Basic copy */
	memset(dest1, 0, sizeof(dest1));
	ft_memcpy(dest1, src, 17);
	check(strcmp(dest1, src) == 0, "ft_memcpy copies string correctly");

	/* 3. Partial copy and boundary check */
	memset(guard_buf, 'Z', sizeof(guard_buf));
	ft_memcpy(guard_buf + 3, "ABCDE", 5);
	check(guard_buf[0] == 'Z' && guard_buf[1] == 'Z' && guard_buf[2] == 'Z',
		"ft_memcpy respects start boundary (prefix untouched)");
	check(memcmp(guard_buf + 3, "ABCDE", 5) == 0,
		"ft_memcpy copies exact middle slice");
	check(guard_buf[8] == 'Z' && guard_buf[9] == 'Z' && guard_buf[19] == 'Z',
		"ft_memcpy respects end boundary (suffix untouched)");

	/* 4. Binary data with embedded '\0' */
	memset(bin_dest, 0xFF, sizeof(bin_dest));
	ft_memcpy(bin_dest, bin_src, 10);
	check(memcmp(bin_dest, bin_src, 10) == 0,
		"ft_memcpy copies binary data with embedded '\\0'");
	check((unsigned char)bin_dest[10] == 0xFF,
		"ft_memcpy stops copying at n bytes in binary data");

	/* 5. n == 0 (must do nothing and return dest) */
	memset(dest1, 'A', sizeof(dest1));
	check(ft_memcpy(dest1, src, 0) == dest1, "ft_memcpy with n = 0 returns dest");
	check(dest1[0] == 'A' && dest1[1] == 'A' && dest1[10] == 'A',
		"ft_memcpy with n = 0 does not modify dest");

	/* 6. Comparison with libc memcpy */
	memset(dest1, 0x55, sizeof(dest1));
	memset(dest2, 0x55, sizeof(dest2));
	ft_memcpy(dest1 + 2, src, 10);
	memcpy(dest2 + 2, src, 10);
	check(memcmp(dest1, dest2, sizeof(dest1)) == 0, "ft_memcpy matches libc memcpy");

	/* 7. Large buffer (1024 bytes) */
	for (i = 0; i < sizeof(large_src); i++)
		large_src[i] = (char)(i % 256);
	ft_memcpy(large_dest, large_src, sizeof(large_src));
	check(memcmp(large_dest, large_src, sizeof(large_src)) == 0,
		"ft_memcpy copies large buffer (1024 bytes)");

	/* 8. NULL pointer handling */
	check(ft_memcpy(NULL, NULL, 0) == NULL, "ft_memcpy(NULL, NULL, 0) == NULL (conforme libc)");
	test_memcpy_null_null();
	check_crash(crash_memcpy_null_dest, "ft_memcpy(NULL, src, 5) raises SIGSEGV (crash attendu)");
	check_crash(crash_memcpy_null_src, "ft_memcpy(dest, NULL, 5) raises SIGSEGV (crash attendu)");
}

