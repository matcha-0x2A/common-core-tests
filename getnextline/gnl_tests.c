#include "gnl_tests.h"

const char *g_failed_names[MAX_FAILED_LOG];
int g_failed_count = 0;
int g_tests_run = 0;
int g_tests_failed = 0;

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

void check_str(const char *got, const char *expected, const char *test_name)
{
	g_tests_run++;
	if ((got == NULL && expected == NULL) || (got && expected && strcmp(got, expected) == 0))
		printf("  [%sOK%s] %s\n", GREEN, RESET, test_name);
	else
	{
		record_failure(test_name);
		printf("  [%sFAIL%s] %s (got: \"%s\", expected: \"%s\")\n",
			RED, RESET, test_name, got ? got : "NULL", expected ? expected : "NULL");
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
		printf("  [%sFAIL%s] %s (got: %p, expected: %p)\n",
			RED, RESET, test_name, got, expected);
	}
}

static void create_temp_file(const char *path, const char *content)
{
	int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
		return;
	if (content && strlen(content) > 0)
		write(fd, content, strlen(content));
	close(fd);
}

void test_invalid_fd(void)
{
	printf("\n%s--- Test Invalid FD ---%s\n", CYAN, RESET);
	check_ptr(get_next_line(-1), NULL, "Negative fd (-1)");
	check_ptr(get_next_line(-42), NULL, "Negative fd (-42)");
	check_ptr(get_next_line(1024), NULL, "Closed high fd (1024)");
}

void test_empty_file(void)
{
	printf("\n%s--- Test Empty File ---%s\n", CYAN, RESET);
	const char *file = "/tmp/gnl_test_empty.txt";
	create_temp_file(file, "");

	int fd = open(file, O_RDONLY);
	if (fd >= 0)
	{
		char *line = get_next_line(fd);
		check_ptr(line, NULL, "First call on empty file returns NULL");
		if (line)
			free(line);
		close(fd);
	}
	unlink(file);
}

void test_single_line_with_nl(void)
{
	printf("\n%s--- Test Single Line With Newline ---%s\n", CYAN, RESET);
	const char *file = "/tmp/gnl_test_single_nl.txt";
	create_temp_file(file, "42 School\n");

	int fd = open(file, O_RDONLY);
	if (fd >= 0)
	{
		char *line = get_next_line(fd);
		check_str(line, "42 School\n", "Read single line with '\\n'");
		if (line)
			free(line);
		line = get_next_line(fd);
		check_ptr(line, NULL, "Second call returns NULL (EOF)");
		if (line)
			free(line);
		close(fd);
	}
	unlink(file);
}

void test_single_line_no_nl(void)
{
	printf("\n%s--- Test Single Line Without Newline ---%s\n", CYAN, RESET);
	const char *file = "/tmp/gnl_test_single_no_nl.txt";
	create_temp_file(file, "No newline at end");

	int fd = open(file, O_RDONLY);
	if (fd >= 0)
	{
		char *line = get_next_line(fd);
		check_str(line, "No newline at end", "Read line ending at EOF without '\\n'");
		if (line)
			free(line);
		line = get_next_line(fd);
		check_ptr(line, NULL, "Second call returns NULL (EOF)");
		if (line)
			free(line);
		close(fd);
	}
	unlink(file);
}

void test_multiple_lines(void)
{
	printf("\n%s--- Test Multiple Lines ---%s\n", CYAN, RESET);
	const char *file = "/tmp/gnl_test_multi.txt";
	create_temp_file(file, "Line 1\nLine 2\nLine 3\n");

	int fd = open(file, O_RDONLY);
	if (fd >= 0)
	{
		char *line1 = get_next_line(fd);
		check_str(line1, "Line 1\n", "Read line 1");
		if (line1)
			free(line1);

		char *line2 = get_next_line(fd);
		check_str(line2, "Line 2\n", "Read line 2");
		if (line2)
			free(line2);

		char *line3 = get_next_line(fd);
		check_str(line3, "Line 3\n", "Read line 3");
		if (line3)
			free(line3);

		char *line4 = get_next_line(fd);
		check_ptr(line4, NULL, "Call 4 returns NULL (EOF)");
		if (line4)
			free(line4);

		close(fd);
	}
	unlink(file);
}

void test_consecutive_newlines(void)
{
	printf("\n%s--- Test Consecutive Newlines ---%s\n", CYAN, RESET);
	const char *file = "/tmp/gnl_test_newlines.txt";
	create_temp_file(file, "\n\n\n");

	int fd = open(file, O_RDONLY);
	if (fd >= 0)
	{
		char *l1 = get_next_line(fd);
		check_str(l1, "\n", "Line 1 is single newline");
		if (l1)
			free(l1);

		char *l2 = get_next_line(fd);
		check_str(l2, "\n", "Line 2 is single newline");
		if (l2)
			free(l2);

		char *l3 = get_next_line(fd);
		check_str(l3, "\n", "Line 3 is single newline");
		if (l3)
			free(l3);

		char *l4 = get_next_line(fd);
		check_ptr(l4, NULL, "Line 4 is NULL (EOF)");
		if (l4)
			free(l4);

		close(fd);
	}
	unlink(file);
}

void test_stdin(void)
{
	printf("\n%s--- Test STDIN (fd = 0, from main.c) ---%s\n", CYAN, RESET);
	int pfd[2];
	if (pipe(pfd) < 0)
		return;
	const char *msg = "stdin line 1\nstdin line 2 without nl";
	if (write(pfd[1], msg, strlen(msg)) < 0)
	{
		close(pfd[0]);
		close(pfd[1]);
		return;
	}
	close(pfd[1]);

	int saved_stdin = dup(STDIN_FILENO);
	dup2(pfd[0], STDIN_FILENO);
	close(pfd[0]);

	char *l1 = get_next_line(STDIN_FILENO);
	check_str(l1, "stdin line 1\n", "Read line 1 from STDIN (fd = 0)");
	if (l1)
		free(l1);

	char *l2 = get_next_line(STDIN_FILENO);
	check_str(l2, "stdin line 2 without nl", "Read line 2 from STDIN (EOF without nl)");
	if (l2)
		free(l2);

	char *l3 = get_next_line(STDIN_FILENO);
	check_ptr(l3, NULL, "Line 3 on STDIN is NULL (EOF)");
	if (l3)
		free(l3);

	dup2(saved_stdin, STDIN_FILENO);
	close(saved_stdin);
}

void test_main_loop_simulation(void)
{
	printf("\n%s--- Test Main Loop Simulation (while loop from main.c) ---%s\n", CYAN, RESET);
	const char *file = "/tmp/gnl_test_main_loop.txt";
	create_temp_file(file, "Alpha\nBeta\nGamma\nDelta");

	int fd = open(file, O_RDONLY);
	if (fd >= 0)
	{
		const char *expected[] = {"Alpha\n", "Beta\n", "Gamma\n", "Delta"};
		int count = 0;
		char *line = get_next_line(fd);
		while (line)
		{
			if (count < 4)
				check_str(line, expected[count], "Main loop read matching line");
			else
				check(0, "Unexpected extra line in main loop");
			free(line);
			count++;
			line = get_next_line(fd);
		}
		check(count == 4, "Total lines read by main loop is 4");
		close(fd);
	}
	unlink(file);
}

int main(void)
{
	printf("%s=== GetNextLine Test Suite (BUFFER_SIZE=%d) ===%s\n", BLUE, BUFFER_SIZE, RESET);

	test_invalid_fd();
	test_empty_file();
	test_single_line_with_nl();
	test_single_line_no_nl();
	test_multiple_lines();
	test_consecutive_newlines();
	test_stdin();
	test_main_loop_simulation();

	printf("\n%s========================================%s\n", BLUE, RESET);
	printf("Tests run: %d | Passed: %s%d%s | Failed: %s%d%s\n",
		g_tests_run,
		GREEN, g_tests_run - g_tests_failed, RESET,
		g_tests_failed > 0 ? RED : GREEN, g_tests_failed, RESET);

	if (g_tests_failed > 0)
	{
		printf("\n%sFailed tests:%s\n", RED, RESET);
		for (int i = 0; i < g_failed_count; i++)
			printf("  - %s\n", g_failed_names[i]);
		return (1);
	}
	printf("%sAll tests passed successfully!%s\n", GREEN, RESET);
	return (0);
}
