#include "libft_tests.h"

static void free_tab(char **tab)
{
	size_t	i;

	if (!tab)
		return ;
	i = 0;
	while (tab[i])
	{
		free(tab[i]);
		i++;
	}
	free(tab);
}

static void test_split_null(void)
{
	pid_t	pid;
	int		status;

	g_tests_run++;
	fflush(stdout);
	pid = fork();
	if (pid < 0)
		return ;
	if (pid == 0)
	{
		char **res = ft_split(NULL, ' ');
		if (res == NULL)
			_exit(0);
		_exit(1);
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
		printf("  [%sOK%s] ft_split(NULL, ' ') == NULL (safe guard)\n", GREEN, RESET);
	else if (WIFSIGNALED(status))
	{
		record_failure("ft_split(NULL, ' ') == NULL (safe guard)");
		printf("  [%sFAIL%s] ft_split(NULL, ' ') crashed with signal %d (expected NULL return)\n", RED, RESET, WTERMSIG(status));
	}
	else
	{
		record_failure("ft_split(NULL, ' ') == NULL (safe guard)");
		printf("  [%sFAIL%s] ft_split(NULL, ' ') returned non-NULL\n", RED, RESET);
	}
}

static void check_split_elem(char **tab, size_t index, const char *expected, const char *test_name)
{
	size_t i = 0;

	if (!tab)
	{
		check(0, test_name);
		return ;
	}
	while (tab[i] && i < index)
		i++;
	if (i == index && tab[index])
		check_str(tab[index], expected, test_name);
	else
	{
		record_failure(test_name);
		g_tests_run++;
		printf("  [%sFAIL%s] %s\n", RED, RESET, test_name);
		printf("         %s-> Expected : \"%s\" at index %zu%s\n", YELLOW, expected ? expected : "NULL", index, RESET);
		printf("         %s-> Got      : (null) / array terminated before index %zu%s\n", RED, index, RESET);
	}
}

static void check_split_null_term(char **tab, size_t index, const char *test_name)
{
	size_t i = 0;

	if (!tab)
	{
		check(0, test_name);
		return ;
	}
	while (tab[i] && i < index)
		i++;
	if (i == index)
		check(tab[index] == NULL, test_name);
	else
	{
		record_failure(test_name);
		g_tests_run++;
		printf("  [%sFAIL%s] %s\n", RED, RESET, test_name);
		printf("         %s-> Expected : NULL terminator at index %zu%s\n", YELLOW, index, RESET);
		printf("         %s-> Got      : array terminated before index %zu%s\n", RED, index, RESET);
	}
}

void test_split(void)
{
	char	**tab;

	printf("\n--- Testing ft_split ---\n");

	/* 1. Standard sentence with spaces */
	tab = ft_split("hello world 42", ' ');
	check(tab != NULL, "ft_split(\"hello world 42\", ' ') returns non-NULL array");
	if (tab)
	{
		check_split_elem(tab, 0, "hello", "ft_split tab[0] == \"hello\"");
		check_split_elem(tab, 1, "world", "ft_split tab[1] == \"world\"");
		check_split_elem(tab, 2, "42", "ft_split tab[2] == \"42\"");
		check_split_null_term(tab, 3, "ft_split tab[3] == NULL (NULL-terminated)");
		free_tab(tab);
	}

	/* 2. Multiple delimiters at start, middle, and end */
	tab = ft_split("---hello---world---42---", '-');
	check(tab != NULL, "ft_split(\"---hello---world---42---\", '-') returns non-NULL array");
	if (tab)
	{
		check_split_elem(tab, 0, "hello", "ft_split leading/multiple delims tab[0] == \"hello\"");
		check_split_elem(tab, 1, "world", "ft_split leading/multiple delims tab[1] == \"world\"");
		check_split_elem(tab, 2, "42", "ft_split leading/multiple delims tab[2] == \"42\"");
		check_split_null_term(tab, 3, "ft_split leading/multiple delims tab[3] == NULL");
		free_tab(tab);
	}

	/* 3. Empty string */
	tab = ft_split("", ' ');
	check(tab != NULL, "ft_split(\"\", ' ') returns non-NULL array");
	if (tab)
	{
		check_split_null_term(tab, 0, "ft_split(\"\", ' ') tab[0] == NULL");
		free_tab(tab);
	}

	/* 4. String containing only delimiters */
	tab = ft_split("     ", ' ');
	check(tab != NULL, "ft_split(\"     \", ' ') returns non-NULL array");
	if (tab)
	{
		check_split_null_term(tab, 0, "ft_split(\"     \", ' ') tab[0] == NULL");
		free_tab(tab);
	}

	/* 5. String with no delimiters */
	tab = ft_split("hello", ' ');
	check(tab != NULL, "ft_split(\"hello\", ' ') returns non-NULL array");
	if (tab)
	{
		check_split_elem(tab, 0, "hello", "ft_split no delimiter tab[0] == \"hello\"");
		check_split_null_term(tab, 1, "ft_split no delimiter tab[1] == NULL");
		free_tab(tab);
	}

	/* 6. Single character word */
	tab = ft_split("a", ' ');
	check(tab != NULL, "ft_split(\"a\", ' ') returns non-NULL array");
	if (tab)
	{
		check_split_elem(tab, 0, "a", "ft_split single char word tab[0] == \"a\"");
		check_split_null_term(tab, 1, "ft_split single char word tab[1] == NULL");
		free_tab(tab);
	}

	/* 7. Single character that is the delimiter */
	tab = ft_split("x", 'x');
	check(tab != NULL, "ft_split(\"x\", 'x') returns non-NULL array");
	if (tab)
	{
		check_split_null_term(tab, 0, "ft_split(\"x\", 'x') tab[0] == NULL");
		free_tab(tab);
	}

	/* 8. Alternating single characters and delimiters */
	tab = ft_split("a b c d", ' ');
	check(tab != NULL, "ft_split(\"a b c d\", ' ') returns non-NULL array");
	if (tab)
	{
		check_split_elem(tab, 0, "a", "tab[0] == \"a\"");
		check_split_elem(tab, 1, "b", "tab[1] == \"b\"");
		check_split_elem(tab, 2, "c", "tab[2] == \"c\"");
		check_split_elem(tab, 3, "d", "tab[3] == \"d\"");
		check_split_null_term(tab, 4, "tab[4] == NULL");
		free_tab(tab);
	}

	/* 9. Delimiter is '\\0' */
	tab = ft_split("hello world", '\0');
	check(tab != NULL, "ft_split(\"hello world\", '\\0') returns non-NULL array");
	if (tab)
	{
		check_split_elem(tab, 0, "hello world", "ft_split with '\\0' delim tab[0] == \"hello world\"");
		check_split_null_term(tab, 1, "ft_split with '\\0' delim tab[1] == NULL");
		free_tab(tab);
	}

	/* 10. NULL string input */
	test_split_null();
}

