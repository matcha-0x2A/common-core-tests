#include "libft_tests.h"

static void crash_bzero_null(void)
{
	ft_bzero(NULL, 5);
}

static void test_bzero_null_zero(void)
{
	pid_t	pid;
	int		status;

	g_tests_run++;
	pid = fork();
	if (pid < 0)
	{
		g_tests_failed++;
		printf("  [%sFAIL%s] ft_bzero(NULL, 0) (fork failed)\n", RED, RESET);
		return ;
	}
	if (pid == 0)
	{
		ft_bzero(NULL, 0);
		exit(0);
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
		printf("  [%sOK%s] ft_bzero(NULL, 0) (no crash)\n", GREEN, RESET);
	else
	{
		g_tests_failed++;
		printf("  [%sFAIL%s] ft_bzero(NULL, 0) crashed\n", RED, RESET);
	}
}

void test_bzero(void)
{
	char	buf1[32];
	char	buf2[32];
	char	guard_buf[16];
	char	large[1024];
	size_t	i;
	int		is_zero;

	printf("\n--- Testing ft_bzero ---\n");

	/* 1. Basic zeroing */
	memset(buf1, 'A', sizeof(buf1));
	ft_bzero(buf1, 10);
	is_zero = 1;
	for (i = 0; i < 10; i++)
	{
		if (buf1[i] != 0)
			is_zero = 0;
	}
	check(is_zero == 1 && buf1[10] == 'A', "ft_bzero zeroes first 10 bytes and stops");

	/* 2. Partial zeroing & boundaries (prefix / suffix untouched) */
	memset(guard_buf, 'X', sizeof(guard_buf));
	ft_bzero(guard_buf + 3, 5);
	check(guard_buf[0] == 'X' && guard_buf[1] == 'X' && guard_buf[2] == 'X',
		"ft_bzero respects start boundary (prefix untouched)");
	is_zero = 1;
	for (i = 3; i < 8; i++)
	{
		if (guard_buf[i] != 0)
			is_zero = 0;
	}
	check(is_zero == 1, "ft_bzero zeroes exact middle slice (5 bytes)");
	check(guard_buf[8] == 'X' && guard_buf[9] == 'X' && guard_buf[15] == 'X',
		"ft_bzero respects end boundary (suffix untouched)");

	/* 3. n == 0 */
	memset(buf1, 'K', sizeof(buf1));
	ft_bzero(buf1, 0);
	check(buf1[0] == 'K' && buf1[1] == 'K' && buf1[15] == 'K',
		"ft_bzero with n = 0 does not modify buffer");

	/* 4. Comparison with libc bzero */
	memset(buf1, 0x42, sizeof(buf1));
	memset(buf2, 0x42, sizeof(buf2));
	ft_bzero(buf1 + 2, 14);
	bzero(buf2 + 2, 14);
	check(memcmp(buf1, buf2, sizeof(buf1)) == 0, "ft_bzero matches libc bzero (slice)");

	/* 5. Large buffer */
	memset(large, 0x7F, sizeof(large));
	ft_bzero(large, sizeof(large));
	is_zero = 1;
	for (i = 0; i < sizeof(large); i++)
	{
		if (large[i] != 0)
			is_zero = 0;
	}
	check(is_zero == 1, "ft_bzero zeroes entire large buffer (1024 bytes)");

	/* 6. NULL pointer handling */
	test_bzero_null_zero();
	check_crash(crash_bzero_null, "ft_bzero(NULL, 5) raises SIGSEGV (crash attendu)");
}

