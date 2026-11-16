#include "libft_tests.h"

const char *g_failed_names[MAX_FAILED_LOG];
int g_failed_count = 0;
int g_tests_run = 0;
int g_tests_failed = 0;

/* Shared list test utilities */
int g_del_count = 0;
void *g_last_del_ptr = NULL;
int g_custom_freed = 0;
int g_del_order[16];
int g_del_order_idx = 0;

void track_del(void *content)
{
	g_del_count++;
	g_last_del_ptr = content;
}

void custom_free_del(void *content)
{
	g_custom_freed++;
	free(content);
}

void order_del(void *content)
{
	if (g_del_order_idx < 16)
		g_del_order[g_del_order_idx++] = (int)(intptr_t)content;
}

void record_failure(const char *test_name)
{
	g_tests_failed++;
	if (g_failed_count < MAX_FAILED_LOG)
		g_failed_names[g_failed_count++] = test_name;
}

void check(int condition, const char *test_name)
{
	g_tests_run++;
	if (condition)
		printf("  [%sOK%s] %s\n", GREEN, RESET, test_name);
	else
	{
		record_failure(test_name);
		printf("  [%sFAIL%s] %s\n", RED, RESET, test_name);
	}
}

void check_int(int got, int expected, const char *test_name)
{
	g_tests_run++;
	if (got == expected)
		printf("  [%sOK%s] %s\n", GREEN, RESET, test_name);
	else
	{
		record_failure(test_name);
		printf("  [%sFAIL%s] %s\n", RED, RESET, test_name);
		printf("         %s-> Expected : %d%s\n", YELLOW, expected, RESET);
		printf("         %s-> Got      : %d%s\n", RED, got, RESET);
	}
}

void check_uint(size_t got, size_t expected, const char *test_name)
{
	g_tests_run++;
	if (got == expected)
		printf("  [%sOK%s] %s\n", GREEN, RESET, test_name);
	else
	{
		record_failure(test_name);
		printf("  [%sFAIL%s] %s\n", RED, RESET, test_name);
		printf("         %s-> Expected : %zu%s\n", YELLOW, expected, RESET);
		printf("         %s-> Got      : %zu%s\n", RED, got, RESET);
	}
}

void check_str(const char *got, const char *expected, const char *test_name)
{
	g_tests_run++;
	if ((got == NULL && expected == NULL) || (got && expected && strcmp(got, expected) == 0))
		printf("  [%sOK%s] %s\n", GREEN, RESET, test_name);
	else
	{
		record_failure(test_name);
		printf("  [%sFAIL%s] %s\n", RED, RESET, test_name);
		printf("         %s-> Expected : \"%s\"%s\n", YELLOW, expected ? expected : "NULL", RESET);
		printf("         %s-> Got      : \"%s\"%s\n", RED, got ? got : "NULL", RESET);
	}
}

void check_ptr(const void *got, const void *expected, const char *test_name)
{
	g_tests_run++;
	if (got == expected)
		printf("  [%sOK%s] %s\n", GREEN, RESET, test_name);
	else
	{
		record_failure(test_name);
		printf("  [%sFAIL%s] %s\n", RED, RESET, test_name);
		printf("         %s-> Expected : %p%s\n", YELLOW, expected, RESET);
		printf("         %s-> Got      : %p%s\n", RED, got, RESET);
	}
}

void check_cmp(int got, int expected, const char *test_name)
{
	g_tests_run++;
	int got_sign = (got > 0) - (got < 0);
	int exp_sign = (expected > 0) - (expected < 0);
	if (got_sign == exp_sign)
		printf("  [%sOK%s] %s\n", GREEN, RESET, test_name);
	else
	{
		record_failure(test_name);
		printf("  [%sFAIL%s] %s\n", RED, RESET, test_name);
		printf("         %s-> Expected sign : %s (libc: %d)%s\n",
			YELLOW, exp_sign > 0 ? "positive (> 0)" : (exp_sign < 0 ? "negative (< 0)" : "zero (0)"),
			expected, RESET);
		printf("         %s-> Got           : %d%s\n", RED, got, RESET);
	}
}

void check_crash(void (*func)(void), const char *test_name)
{
	pid_t	pid;
	int		status;

	g_tests_run++;
	fflush(stdout);
	pid = fork();
	if (pid < 0)
	{
		record_failure(test_name);
		printf("  [%sFAIL%s] %s (fork failed)\n", RED, RESET, test_name);
		return ;
	}
	if (pid == 0)
	{
		func();
		_exit(0);
	}
	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGSEGV)
		printf("  [%sOK%s] %s\n", GREEN, RESET, test_name);
	else
	{
		record_failure(test_name);
		printf("  [%sFAIL%s] %s (did NOT crash with SIGSEGV as expected)\n", RED, RESET, test_name);
	}
}

void check_no_crash(void (*func)(void), const char *test_name)
{
	pid_t	pid;
	int		status;

	g_tests_run++;
	fflush(stdout);
	pid = fork();
	if (pid < 0)
	{
		record_failure(test_name);
		printf("  [%sFAIL%s] %s (fork failed)\n", RED, RESET, test_name);
		return ;
	}
	if (pid == 0)
	{
		func();
		_exit(0);
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
		printf("  [%sOK%s] %s\n", GREEN, RESET, test_name);
	else
	{
		record_failure(test_name);
		if (WIFSIGNALED(status))
			printf("  [%sFAIL%s] %s (crashed with signal %d)\n", RED, RESET, test_name, WTERMSIG(status));
		else
			printf("  [%sFAIL%s] %s (exited with error code %d)\n", RED, RESET, test_name, WEXITSTATUS(status));
	}
}

static void run_test_module(void (*func)(void), const char *module_name)
{
	int before;
	int diff;

	before = get_active_allocs();
	func();
	diff = get_active_allocs() - before;
	g_tests_run++;
	if (diff == 0)
		printf("  [%sOK%s] Memory audit: %s leaves 0 memory leaks\n", GREEN, RESET, module_name);
	else
	{
		g_tests_failed++;
		record_failure(module_name);
		printf("  [%sFAIL%s] Memory audit: %s leaked %d block(s)\n", RED, RESET, module_name, diff);
	}
}

#define RUN_TEST(fn) run_test_module(fn, #fn)

int main(void)
{
	printf("=========================================\n");
	printf("   LIBFT TESTS: classification, case & mem\n");
	printf("=========================================\n");

	RUN_TEST(test_isalpha);
	RUN_TEST(test_isdigit);
	RUN_TEST(test_isalnum);
	RUN_TEST(test_isascii);
	RUN_TEST(test_isprint);
	RUN_TEST(test_toupper);
	RUN_TEST(test_tolower);
	RUN_TEST(test_strlen);
	RUN_TEST(test_memset);
	RUN_TEST(test_bzero);
	RUN_TEST(test_memcpy);
	RUN_TEST(test_memmove);
	RUN_TEST(test_memchr);
	RUN_TEST(test_calloc);
	RUN_TEST(test_strdup);
	RUN_TEST(test_strchr);
	RUN_TEST(test_strrchr);
	RUN_TEST(test_strlcpy);
	RUN_TEST(test_strlcat);
	RUN_TEST(test_strncmp);
	RUN_TEST(test_memcmp);
	RUN_TEST(test_strnstr);
	RUN_TEST(test_atoi);

	printf("\n=========================================\n");
	printf("   LIBFT TESTS: Part 3 - Linked Lists\n");
	printf("=========================================\n");
	RUN_TEST(test_lstnew);
	RUN_TEST(test_lstadd_front);
	RUN_TEST(test_lstsize);
	RUN_TEST(test_lstlast);
	RUN_TEST(test_lstadd_back);
	RUN_TEST(test_lstdelone);
	RUN_TEST(test_lstclear);
	RUN_TEST(test_lstiter);
	RUN_TEST(test_lstmap);
	RUN_TEST(test_lst_lifecycle);

	printf("\n=========================================\n");
	printf("   LIBFT TESTS: Part 2 - Additional Functions\n");
	printf("=========================================\n");
	RUN_TEST(test_substr);
	RUN_TEST(test_split);
	RUN_TEST(test_strjoin);
	RUN_TEST(test_strtrim);
	RUN_TEST(test_itoa);
	RUN_TEST(test_strmapi);
	RUN_TEST(test_striteri);
	RUN_TEST(test_putnbr_fd);

	printf("\n=========================================\n");
	printf("   LIBFT TESTS: Global Memory Audit\n");
	printf("=========================================\n");
	check(get_active_allocs() == 0, "Global heap audit: 0 memory leaks across all test cases");

	printf("\n=========================================\n");
	if (g_tests_failed == 0)
		printf("%sALL %d TESTS PASSED SUCCESSFULLY!%s\n", GREEN, g_tests_run, RESET);
	else
	{
		int i;

		printf("%s%d / %d TESTS FAILED!%s\n", RED, g_tests_failed, g_tests_run, RESET);
		printf("=========================================\n");
		printf("\n%sFAILED TESTS SUMMARY (%d failure%s):%s\n", RED, g_failed_count, g_failed_count > 1 ? "s" : "", RESET);
		for (i = 0; i < g_failed_count; i++)
			printf("  %s✗%s %s\n", RED, RESET, g_failed_names[i]);
	}
	printf("=========================================\n");

	return (g_tests_failed != 0);
}
