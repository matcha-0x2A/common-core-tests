#include "libft_tests.h"

void test_calloc(void)
{
	void	*ptr;
	char	*cptr;
	int		*iptr;
	size_t	i;
	int		is_zero;
	pid_t	pid;
	int		status;

	printf("\n--- Testing ft_calloc ---\n");

	/* 1. Basic allocation and zero initialization */
	ptr = ft_calloc(10, sizeof(char));
	check(ptr != NULL, "ft_calloc(10, sizeof(char)) returns non-NULL");
	if (ptr)
	{
		cptr = (char *)ptr;
		is_zero = 1;
		for (i = 0; i < 10; i++)
		{
			if (cptr[i] != 0)
				is_zero = 0;
		}
		check(is_zero == 1, "ft_calloc memory is fully zero-initialized (char)");
		free(ptr);
	}

	/* 2. Larger types (int array) */
	iptr = (int *)ft_calloc(25, sizeof(int));
	check(iptr != NULL, "ft_calloc(25, sizeof(int)) returns non-NULL");
	if (iptr)
	{
		is_zero = 1;
		for (i = 0; i < 25; i++)
		{
			if (iptr[i] != 0)
				is_zero = 0;
		}
		check(is_zero == 1, "ft_calloc memory is fully zero-initialized (int)");
		/* Test writability */
		for (i = 0; i < 25; i++)
			iptr[i] = (int)(i * 42);
		check(iptr[0] == 0 && iptr[24] == 24 * 42, "ft_calloc memory is writable and readable");
		free(iptr);
	}

	/* 3. Larger allocation (4096 bytes) */
	ptr = ft_calloc(4096, 1);
	check(ptr != NULL, "ft_calloc(4096, 1) returns non-NULL");
	if (ptr)
	{
		cptr = (char *)ptr;
		is_zero = 1;
		for (i = 0; i < 4096; i++)
		{
			if (cptr[i] != 0)
				is_zero = 0;
		}
		check(is_zero == 1, "ft_calloc(4096, 1) all bytes are 0");
		free(ptr);
	}

	/* 4. Zero size allocations (nmemb = 0 or size = 0) */
	/* According to man: returns NULL or unique pointer that can be freed without error */
	ptr = ft_calloc(0, 10);
	g_tests_run++;
	pid = fork();
	if (pid == 0)
	{
		free(ptr);
		exit(0);
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
		printf("  [%sOK%s] ft_calloc(0, 10) can be freed safely\n", GREEN, RESET);
	else
	{
		g_tests_failed++;
		printf("  [%sFAIL%s] ft_calloc(0, 10) failed to free\n", RED, RESET);
	}
	free(ptr);

	ptr = ft_calloc(10, 0);
	g_tests_run++;
	pid = fork();
	if (pid == 0)
	{
		free(ptr);
		exit(0);
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
		printf("  [%sOK%s] ft_calloc(10, 0) can be freed safely\n", GREEN, RESET);
	else
	{
		g_tests_failed++;
		printf("  [%sFAIL%s] ft_calloc(10, 0) failed to free\n", RED, RESET);
	}
	free(ptr);

	ptr = ft_calloc(0, 0);
	g_tests_run++;
	pid = fork();
	if (pid == 0)
	{
		free(ptr);
		exit(0);
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
		printf("  [%sOK%s] ft_calloc(0, 0) can be freed safely\n", GREEN, RESET);
	else
	{
		g_tests_failed++;
		printf("  [%sFAIL%s] ft_calloc(0, 0) failed to free\n", RED, RESET);
	}
	free(ptr);

	/* 5. Integer overflow protection on nmemb * size (Crucial 42 / security check) */
	check(ft_calloc(SIZE_MAX, SIZE_MAX) == NULL,
		"ft_calloc(SIZE_MAX, SIZE_MAX) returns NULL (overflow protection)");
	check(ft_calloc(SIZE_MAX / 2 + 1, 2) == NULL,
		"ft_calloc(SIZE_MAX / 2 + 1, 2) returns NULL (overflow protection)");
	check(ft_calloc(2, SIZE_MAX / 2 + 1) == NULL,
		"ft_calloc(2, SIZE_MAX / 2 + 1) returns NULL (overflow protection)");
	check(ft_calloc(SIZE_MAX, 1) == NULL,
		"ft_calloc(SIZE_MAX, 1) returns NULL (impossible size)");
	check(ft_calloc(1, SIZE_MAX) == NULL,
		"ft_calloc(1, SIZE_MAX) returns NULL (impossible size)");
}

