#include "libft_tests.h"

static t_shared_state g_fallback_shm;
t_shared_state *g_shm = &g_fallback_shm;

static void __attribute__((constructor)) init_shared_state(void)
{
	void *ptr = mmap(NULL, sizeof(t_shared_state), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (ptr != MAP_FAILED)
	{
		g_shm = (t_shared_state *)ptr;
		memset(g_shm, 0, sizeof(t_shared_state));
	}
}

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
	if (!g_shm)
		return ;
	g_shm->tests_failed++;
	if (g_shm->failed_count < MAX_FAILED_LOG)
	{
		strncpy(g_shm->failed_names[g_shm->failed_count], test_name, 127);
		g_shm->failed_names[g_shm->failed_count][127] = '\0';
		g_shm->failed_count++;
	}
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

void check_malloc_nth_protection(void *(*func)(void), int nth, const char *test_name)
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
		set_malloc_dirty(1);
		int before = get_active_allocs();
		set_malloc_failure(nth);
		void *res = func();
		reset_malloc_failure();
		if (res != NULL)
		{
			free(res);
			_exit(1);
		}
		int after = get_active_allocs();
		if (after != before)
			_exit(2);
		_exit(0);
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
		printf("  [%sOK%s] %s\n", GREEN, RESET, test_name);
	else if (WIFSIGNALED(status))
	{
		record_failure(test_name);
		printf("  [%sFAIL%s] %s (crashed with signal %d)\n", RED, RESET, test_name, WTERMSIG(status));
	}
	else if (WEXITSTATUS(status) == 1)
	{
		record_failure(test_name);
		printf("  [%sFAIL%s] %s (expected NULL return, got non-NULL)\n", RED, RESET, test_name);
	}
	else if (WEXITSTATUS(status) == 2)
	{
		record_failure(test_name);
		printf("  [%sFAIL%s] %s (memory leak: allocated blocks not freed on error)\n", RED, RESET, test_name);
	}
	else
	{
		record_failure(test_name);
		printf("  [%sFAIL%s] %s (exited with error code %d)\n", RED, RESET, test_name, WEXITSTATUS(status));
	}
}

void check_malloc_protection(void *(*func)(void), const char *test_name)
{
	check_malloc_nth_protection(func, 1, test_name);
}

static void run_test_module(void (*func)(void), const char *module_name)
{
	pid_t	pid;
	int		status;

	fflush(stdout);
	fflush(stderr);
	pid = fork();
	if (pid < 0)
	{
		g_tests_run++;
		record_failure(module_name);
		printf("  [%sFAIL%s] %s (fork failed)\n", RED, RESET, module_name);
		return ;
	}
	if (pid == 0)
	{
		alarm(30);
		int before = get_active_allocs();
		func();
		int diff = get_active_allocs() - before;
		g_tests_run++;
		if (diff == 0)
			printf("  [%sOK%s] Memory audit: %s leaves 0 memory leaks\n", GREEN, RESET, module_name);
		else
		{
			record_failure(module_name);
			printf("  [%sFAIL%s] Memory audit: %s leaked %d block(s)\n", RED, RESET, module_name, diff);
		}
		fflush(stdout);
		_exit(0);
	}
	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status))
	{
		int sig = WTERMSIG(status);
		g_tests_run++;
		char crash_buf[128];
		if (sig == SIGALRM)
			snprintf(crash_buf, sizeof(crash_buf), "%s (timed out / infinite loop)", module_name);
		else
			snprintf(crash_buf, sizeof(crash_buf), "%s (crashed with signal %d)", module_name, sig);
		record_failure(crash_buf);
		if (sig == SIGALRM)
			printf("  [%sFAIL%s] %s (timed out after 30s / infinite loop)\n", RED, RESET, module_name);
		else
			printf("  [%sFAIL%s] %s (crashed with signal %d)\n", RED, RESET, module_name, sig);
	}
	else if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
	{
		g_tests_run++;
		char exit_buf[128];
		snprintf(exit_buf, sizeof(exit_buf), "%s (exited with error code %d)", module_name, WEXITSTATUS(status));
		record_failure(exit_buf);
		printf("  [%sFAIL%s] %s (exited with error code %d)\n", RED, RESET, module_name, WEXITSTATUS(status));
	}
}

typedef struct s_test_entry {
	const char	*name;
	const char	*alt_name;
	void		(*func)(void);
	int			section;
} t_test_entry;

static const t_test_entry g_test_entries[] = {
	/* Section 1: classification, case & mem */
	{"ft_isalpha", "isalpha", test_isalpha, 1},
	{"ft_isdigit", "isdigit", test_isdigit, 1},
	{"ft_isalnum", "isalnum", test_isalnum, 1},
	{"ft_isascii", "isascii", test_isascii, 1},
	{"ft_isprint", "isprint", test_isprint, 1},
	{"ft_toupper", "toupper", test_toupper, 1},
	{"ft_tolower", "tolower", test_tolower, 1},
	{"ft_strlen", "strlen", test_strlen, 1},
	{"ft_memset", "memset", test_memset, 1},
	{"ft_bzero", "bzero", test_bzero, 1},
	{"ft_memcpy", "memcpy", test_memcpy, 1},
	{"ft_memmove", "memmove", test_memmove, 1},
	{"ft_memchr", "memchr", test_memchr, 1},
	{"ft_calloc", "calloc", test_calloc, 1},
	{"ft_strdup", "strdup", test_strdup, 1},
	{"ft_strchr", "strchr", test_strchr, 1},
	{"ft_strrchr", "strrchr", test_strrchr, 1},
	{"ft_strlcpy", "strlcpy", test_strlcpy, 1},
	{"ft_strlcat", "strlcat", test_strlcat, 1},
	{"ft_strncmp", "strncmp", test_strncmp, 1},
	{"ft_memcmp", "memcmp", test_memcmp, 1},
	{"ft_strnstr", "strnstr", test_strnstr, 1},
	{"ft_atoi", "atoi", test_atoi, 1},

	/* Section 3: Part 3 - Linked Lists */
	{"ft_lstnew", "lstnew", test_lstnew, 3},
	{"ft_lstadd_front", "lstadd_front", test_lstadd_front, 3},
	{"ft_lstsize", "lstsize", test_lstsize, 3},
	{"ft_lstlast", "lstlast", test_lstlast, 3},
	{"ft_lstadd_back", "lstadd_back", test_lstadd_back, 3},
	{"ft_lstdelone", "lstdelone", test_lstdelone, 3},
	{"ft_lstclear", "lstclear", test_lstclear, 3},
	{"ft_lstiter", "lstiter", test_lstiter, 3},
	{"ft_lstmap", "lstmap", test_lstmap, 3},
	{"ft_lst_lifecycle", "lst_lifecycle", test_lst_lifecycle, 3},

	/* Section 2: Part 2 - Additional Functions */
	{"ft_substr", "substr", test_substr, 2},
	{"ft_split", "split", test_split, 2},
	{"ft_strjoin", "strjoin", test_strjoin, 2},
	{"ft_strtrim", "strtrim", test_strtrim, 2},
	{"ft_itoa", "itoa", test_itoa, 2},
	{"ft_strmapi", "strmapi", test_strmapi, 2},
	{"ft_striteri", "striteri", test_striteri, 2},
	{"ft_putnbr_fd", "putnbr_fd", test_putnbr_fd, 2}
};

static int match_filter(const t_test_entry *entry, const char *filter)
{
	if (strcasecmp(filter, "all") == 0)
		return (1);
	if (strcasecmp(filter, "part1") == 0 || strcasecmp(filter, "mandatory") == 0)
		return (entry->section == 1 || entry->section == 2);
	if (strcasecmp(filter, "part2") == 0 || strcasecmp(filter, "additional") == 0)
		return (entry->section == 2);
	if (strcasecmp(filter, "part3") == 0 || strcasecmp(filter, "bonus") == 0 || strcasecmp(filter, "list") == 0 || strcasecmp(filter, "lists") == 0)
		return (entry->section == 3);
	if (strcasecmp(filter, entry->name) == 0 || strcasecmp(filter, entry->alt_name) == 0)
		return (1);
	if (strncasecmp(filter, "test_", 5) == 0)
	{
		const char *sub = filter + 5;
		if (strcasecmp(sub, entry->name) == 0 || strcasecmp(sub, entry->alt_name) == 0)
			return (1);
	}
	return (0);
}

int main(int argc, char **argv)
{
	int num_entries = sizeof(g_test_entries) / sizeof(g_test_entries[0]);
	int to_run[num_entries];
	int run_all = (argc <= 1);

	for (int i = 0; i < num_entries; i++)
		to_run[i] = run_all ? 1 : 0;

	if (!run_all)
	{
		for (int a = 1; a < argc; a++)
		{
			if (strcmp(argv[a], "--help") == 0 || strcmp(argv[a], "-h") == 0)
			{
				printf("Usage: %s [function_name ...] [part1|part2|part3|bonus|all]\n", argv[0]);
				printf("Examples:\n");
				printf("  %s ft_strlcat\n", argv[0]);
				printf("  %s strlcat strlen atoi\n", argv[0]);
				printf("  %s bonus\n", argv[0]);
				return (0);
			}

			int matched_arg = 0;
			for (int i = 0; i < num_entries; i++)
			{
				if (match_filter(&g_test_entries[i], argv[a]))
				{
					to_run[i] = 1;
					matched_arg = 1;
				}
			}
			if (!matched_arg)
			{
				printf("%sError: unknown test or function filter '%s'%s\n", RED, argv[a], RESET);
				printf("Available functions:\n ");
				for (int i = 0; i < num_entries; i++)
					printf(" %s", g_test_entries[i].alt_name);
				printf("\nGroups: part1, part2, part3 / bonus, all\n");
				return (1);
			}
		}
	}


	int last_section = -1;
	for (int i = 0; i < num_entries; i++)
	{
		if (!to_run[i])
			continue;

		if (g_test_entries[i].section != last_section)
		{
			last_section = g_test_entries[i].section;
			if (last_section == 1)
			{
				printf("=========================================\n");
				printf("   LIBFT TESTS: classification, case & mem\n");
				printf("=========================================\n");
			}
			else if (last_section == 3)
			{
				printf("\n=========================================\n");
				printf("   LIBFT TESTS: Part 3 - Linked Lists\n");
				printf("=========================================\n");
			}
			else if (last_section == 2)
			{
				printf("\n=========================================\n");
				printf("   LIBFT TESTS: Part 2 - Additional Functions\n");
				printf("=========================================\n");
			}
		}
		run_test_module(g_test_entries[i].func, g_test_entries[i].name);
	}

	if (run_all)
	{
		printf("\n=========================================\n");
		printf("   LIBFT TESTS: Global Memory Audit\n");
		printf("=========================================\n");
		check(get_active_allocs() == 0, "Global heap audit: 0 memory leaks across all test cases");
	}

	printf("\n=========================================\n");
	if (g_shm->tests_failed == 0)
		printf("%sALL %d TESTS PASSED SUCCESSFULLY!%s\n", GREEN, g_shm->tests_run, RESET);
	else
	{
		int i;

		printf("%s%d / %d TESTS FAILED!%s\n", RED, g_shm->tests_failed, g_shm->tests_run, RESET);
		printf("=========================================\n");
		printf("\n%sFAILED TESTS SUMMARY (%d failure%s):%s\n", RED, g_shm->failed_count, g_shm->failed_count > 1 ? "s" : "", RESET);
		for (i = 0; i < g_shm->failed_count; i++)
			printf("  %s✗%s %s\n", RED, RESET, g_shm->failed_names[i]);
	}
	printf("=========================================\n");

	return (g_shm->tests_failed != 0);
}
