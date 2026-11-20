#include "libft_tests.h"

static void crash_memmove_null_dest(void)
{
	char	src[] = "hello";

	ft_memmove(NULL, src, 5);
}

static void crash_memmove_null_src(void)
{
	char	dest[10];

	ft_memmove(dest, NULL, 5);
}

static void test_memmove_null_null(void)
{
	pid_t	pid;
	int		status;

	g_tests_run++;
	fflush(stdout);
	pid = fork();
	if (pid < 0)
	{
		g_tests_failed++;
		printf("  [%sFAIL%s] ft_memmove(NULL, NULL, 3) (fork failed)\n", RED, RESET);
		return ;
	}
	if (pid == 0)
	{
		void *res = ft_memmove(NULL, NULL, 3);
		if (res == NULL)
			_exit(0);
		_exit(1);
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
		printf("  [%sOK%s] ft_memmove(NULL, NULL, 3) == NULL (conforme 42)\n", GREEN, RESET);
	else if (WIFSIGNALED(status) && WTERMSIG(status) == SIGSEGV)
		printf("  [%sOK%s] ft_memmove(NULL, NULL, 3) crashed with signal %d (conforme libc)\n", GREEN, RESET, WTERMSIG(status));
	else if (WIFSIGNALED(status))
	{
		g_tests_failed++;
		printf("  [%sFAIL%s] ft_memmove(NULL, NULL, 3) crashed with signal %d (expected NULL ou SIGSEGV)\n", RED, RESET, WTERMSIG(status));
	}
	else
	{
		g_tests_failed++;
		printf("  [%sFAIL%s] ft_memmove(NULL, NULL, 3) returned non-NULL\n", RED, RESET);
	}
}

void test_memmove(void)
{
	char	src[] = "Hello, 42 World!";
	char	dest1[32];
	char	guard_buf[20];
	char	bin_src[] = {'4', '2', '\0', 'T', 'o', 'k', 'y', 'o', '\0', '!'};
	char	bin_dest[16];
	char	overlap_buf1[64];
	char	overlap_buf2[64];
	char	large_src[1024];
	char	large_dest[1024];
	size_t	i;

	printf("\n--- Testing ft_memmove ---\n");

	/* 1. Return value: must return dest pointer */
	check(ft_memmove(dest1, src, 16) == dest1, "ft_memmove returns dest pointer");

	/* 2. Basic non-overlapping copy */
	memset(dest1, 0, sizeof(dest1));
	ft_memmove(dest1, src, 17);
	check(strcmp(dest1, src) == 0, "ft_memmove copies string correctly");

	/* 3. Partial copy and boundary check */
	memset(guard_buf, 'Z', sizeof(guard_buf));
	ft_memmove(guard_buf + 3, "ABCDE", 5);
	check(guard_buf[0] == 'Z' && guard_buf[1] == 'Z' && guard_buf[2] == 'Z',
		"ft_memmove respects start boundary (prefix untouched)");
	check(memcmp(guard_buf + 3, "ABCDE", 5) == 0,
		"ft_memmove copies exact middle slice");
	check(guard_buf[8] == 'Z' && guard_buf[9] == 'Z' && guard_buf[19] == 'Z',
		"ft_memmove respects end boundary (suffix untouched)");

	/* 4. Binary data with embedded '\\0' */
	memset(bin_dest, 0xFF, sizeof(bin_dest));
	ft_memmove(bin_dest, bin_src, 10);
	check(memcmp(bin_dest, bin_src, 10) == 0,
		"ft_memmove copies binary data with embedded '\\0'");
	check((unsigned char)bin_dest[10] == 0xFF,
		"ft_memmove stops copying at n bytes in binary data");

	/* 5. n == 0 (must do nothing and return dest) */
	memset(dest1, 'A', sizeof(dest1));
	check(ft_memmove(dest1, src, 0) == dest1, "ft_memmove with n = 0 returns dest");
	check(dest1[0] == 'A' && dest1[1] == 'A' && dest1[10] == 'A',
		"ft_memmove with n = 0 does not modify dest");

	/* 6. Overlapping: dest > src (shift right / backward copy) */
	strcpy(overlap_buf1, "abcdefghijklmnopqrstuvwxyz");
	strcpy(overlap_buf2, "abcdefghijklmnopqrstuvwxyz");
	ft_memmove(overlap_buf1 + 5, overlap_buf1, 15);
	memmove(overlap_buf2 + 5, overlap_buf2, 15);
	check(memcmp(overlap_buf1, overlap_buf2, 27) == 0,
		"ft_memmove handles dest > src overlap (shift right)");

	/* 7. Overlapping: dest < src (shift left / forward copy) */
	strcpy(overlap_buf1, "abcdefghijklmnopqrstuvwxyz");
	strcpy(overlap_buf2, "abcdefghijklmnopqrstuvwxyz");
	ft_memmove(overlap_buf1, overlap_buf1 + 5, 15);
	memmove(overlap_buf2, overlap_buf2 + 5, 15);
	check(memcmp(overlap_buf1, overlap_buf2, 27) == 0,
		"ft_memmove handles dest < src overlap (shift left)");

	/* 8. Overlapping: 1-byte shift right (critical overlap) */
	strcpy(overlap_buf1, "0123456789ABCDEF");
	strcpy(overlap_buf2, "0123456789ABCDEF");
	ft_memmove(overlap_buf1 + 1, overlap_buf1, 15);
	memmove(overlap_buf2 + 1, overlap_buf2, 15);
	check(memcmp(overlap_buf1, overlap_buf2, 17) == 0,
		"ft_memmove handles 1-byte shift right overlap");

	/* 9. Overlapping: 1-byte shift left (critical overlap) */
	strcpy(overlap_buf1, "0123456789ABCDEF");
	strcpy(overlap_buf2, "0123456789ABCDEF");
	ft_memmove(overlap_buf1, overlap_buf1 + 1, 15);
	memmove(overlap_buf2, overlap_buf2 + 1, 15);
	check(memcmp(overlap_buf1, overlap_buf2, 17) == 0,
		"ft_memmove handles 1-byte shift left overlap");

	/* 10. dest == src */
	strcpy(overlap_buf1, "same_pointer");
	check(ft_memmove(overlap_buf1, overlap_buf1, 12) == overlap_buf1,
		"ft_memmove with dest == src returns dest");
	check(strcmp(overlap_buf1, "same_pointer") == 0,
		"ft_memmove with dest == src does not modify buffer");

	/* 11. Large buffer (1024 bytes) overlapping */
	for (i = 0; i < sizeof(large_src); i++)
		large_src[i] = (char)(i % 256);
	memcpy(large_dest, large_src, sizeof(large_src));
	ft_memmove(large_src + 50, large_src, 900);
	memmove(large_dest + 50, large_dest, 900);
	check(memcmp(large_src, large_dest, sizeof(large_src)) == 0,
		"ft_memmove matches libc memmove on large overlapping buffer (1024 bytes)");

	/* 12. NULL pointer handling */
	check(ft_memmove(NULL, NULL, 0) == NULL, "ft_memmove(NULL, NULL, 0) == NULL (conforme libc)");
	test_memmove_null_null();
	check_crash(crash_memmove_null_dest, "ft_memmove(NULL, src, 5) raises SIGSEGV (crash attendu)");
	check_crash(crash_memmove_null_src, "ft_memmove(dest, NULL, 5) raises SIGSEGV (crash attendu)");
}

