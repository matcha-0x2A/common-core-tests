#include "printf_tests.h"

const char *g_failed_names[MAX_FAILED_LOG];
t_failed_test g_failed_tests[MAX_FAILED_LOG];
int g_failed_count = 0;
int g_tests_run = 0;
int g_tests_failed = 0;

void print_escaped(const char *buf, int len)
{
	printf("\"");
	for (int i = 0; i < len; i++)
	{
		unsigned char c = (unsigned char)buf[i];
		if (c == '\n')
			printf("\\n");
		else if (c == '\t')
			printf("\\t");
		else if (c == '\r')
			printf("\\r");
		else if (c == '\0')
			printf("\\0");
		else if (c == '\\')
			printf("\\\\");
		else if (c == '\"')
			printf("\\\"");
		else if (c >= 32 && c <= 126)
			putchar(c);
		else
			printf("\\x%02x", c);
	}
	printf("\"");
}

void record_failure(const char *test_name, const t_capture *got, const t_capture *exp)
{
	g_tests_failed++;
	if (g_failed_count < MAX_FAILED_LOG)
	{
		t_failed_test *f = &g_failed_tests[g_failed_count];
		strncpy(f->name, test_name, sizeof(f->name) - 1);
		f->name[sizeof(f->name) - 1] = '\0';

		int elen = exp->len < (int)sizeof(f->exp_out) - 1 ? exp->len : (int)sizeof(f->exp_out) - 1;
		memcpy(f->exp_out, exp->buf, elen);
		f->exp_out[elen] = '\0';
		f->exp_out_len = elen;
		f->exp_ret = exp->ret;

		int glen = got->len < (int)sizeof(f->got_out) - 1 ? got->len : (int)sizeof(f->got_out) - 1;
		memcpy(f->got_out, got->buf, glen);
		f->got_out[glen] = '\0';
		f->got_out_len = glen;
		f->got_ret = got->ret;
		f->is_int_only = 0;

		g_failed_names[g_failed_count] = f->name;
		g_failed_count++;
	}
}

void record_failure_int(const char *test_name, int got, int expected)
{
	g_tests_failed++;
	if (g_failed_count < MAX_FAILED_LOG)
	{
		t_failed_test *f = &g_failed_tests[g_failed_count];
		strncpy(f->name, test_name, sizeof(f->name) - 1);
		f->name[sizeof(f->name) - 1] = '\0';
		f->exp_out[0] = '\0';
		f->exp_out_len = 0;
		f->exp_ret = expected;
		f->got_out[0] = '\0';
		f->got_out_len = 0;
		f->got_ret = got;
		f->is_int_only = 1;

		g_failed_names[g_failed_count] = f->name;
		g_failed_count++;
	}
}

void record_failure_simple(const char *test_name)
{
	g_tests_failed++;
	if (g_failed_count < MAX_FAILED_LOG)
	{
		t_failed_test *f = &g_failed_tests[g_failed_count];
		strncpy(f->name, test_name, sizeof(f->name) - 1);
		f->name[sizeof(f->name) - 1] = '\0';
		f->exp_out[0] = '\0';
		f->exp_out_len = 0;
		f->exp_ret = 0;
		f->got_out[0] = '\0';
		f->got_out_len = 0;
		f->got_ret = 0;
		f->is_int_only = 0;

		g_failed_names[g_failed_count] = f->name;
		g_failed_count++;
	}
}

void capture_call_start(int pfd[2], int *saved_stdout)
{
	fflush(stdout);
	*saved_stdout = dup(STDOUT_FILENO);
	if (pipe(pfd) == -1)
		return;
	dup2(pfd[1], STDOUT_FILENO);
	close(pfd[1]);
}

void capture_call_end(int pfd[2], int saved_stdout, t_capture *cap, int ret)
{
	fflush(stdout);
	dup2(saved_stdout, STDOUT_FILENO);
	close(saved_stdout);

	memset(cap->buf, 0, sizeof(cap->buf));
	ssize_t n = read(pfd[0], cap->buf, sizeof(cap->buf) - 1);
	close(pfd[0]);
	if (n < 0)
		n = 0;
	cap->len = (int)n;
	cap->buf[cap->len] = '\0';
	cap->ret = ret;
}

void compare_and_report(const char *test_name, const t_capture *got, const t_capture *exp)
{
	g_tests_run++;
	int match = (got->ret == exp->ret)
		&& (got->len == exp->len)
		&& (memcmp(got->buf, exp->buf, got->len) == 0);

	if (match)
	{
		printf("  [%sOK%s] %s\n", GREEN, RESET, test_name);
	}
	else
	{
		record_failure(test_name, got, exp);
		printf("  [%sFAIL%s] %s\n", RED, RESET, test_name);
		printf("         %s-> Expected output : %s", YELLOW, RESET);
		print_escaped(exp->buf, exp->len);
		printf(" (len: %d)\n", exp->len);
		printf("         %s-> Expected return : %d%s\n", YELLOW, exp->ret, RESET);
		printf("         %s-> Got output      : %s", RED, RESET);
		print_escaped(got->buf, got->len);
		printf(" (len: %d)\n", got->len);
		printf("         %s-> Got return      : %d%s\n", RED, got->ret, RESET);
	}
}

void check(int condition, const char *test_name)
{
	g_tests_run++;
	if (condition)
		printf("  [%sOK%s] %s\n", GREEN, RESET, test_name);
	else
	{
		record_failure_simple(test_name);
		printf("  [%sFAIL%s] %s\n", RED, RESET, test_name);
	}
}

void check_int(int got, int expected, const char *test_name)
{
	g_tests_run++;
	if (got == expected)
		printf("  [%sOK%s] %s (return: %d)\n", GREEN, RESET, test_name, got);
	else
	{
		record_failure_int(test_name, got, expected);
		printf("  [%sFAIL%s] %s\n", RED, RESET, test_name);
		printf("         %s-> Expected return : %d%s\n", YELLOW, expected, RESET);
		printf("         %s-> Got return      : %d%s\n", RED, got, RESET);
	}
}

void check_str(const char *got, const char *expected, const char *test_name)
{
	g_tests_run++;
	if ((got == NULL && expected == NULL) || (got && expected && strcmp(got, expected) == 0))
		printf("  [%sOK%s] %s\n", GREEN, RESET, test_name);
	else
	{
		t_capture c_got;
		t_capture c_exp;
		memset(&c_got, 0, sizeof(c_got));
		memset(&c_exp, 0, sizeof(c_exp));
		if (got)
		{
			size_t l = strlen(got);
			if (l >= sizeof(c_got.buf)) l = sizeof(c_got.buf) - 1;
			memcpy(c_got.buf, got, l);
			c_got.len = (int)l;
			c_got.ret = (int)l;
		}
		if (expected)
		{
			size_t l = strlen(expected);
			if (l >= sizeof(c_exp.buf)) l = sizeof(c_exp.buf) - 1;
			memcpy(c_exp.buf, expected, l);
			c_exp.len = (int)l;
			c_exp.ret = (int)l;
		}
		record_failure(test_name, &c_got, &c_exp);
		printf("  [%sFAIL%s] %s\n", RED, RESET, test_name);
		printf("         %s-> Expected output : \"%s\"%s\n", YELLOW, expected ? expected : "NULL", RESET);
		printf("         %s-> Got output      : \"%s\"%s\n", RED, got ? got : "NULL", RESET);
	}
}

void test_char(void)
{
	printf("\n%s--- Test %%c (Characters) ---%s\n", CYAN, RESET);
	TEST_PRINTF("%c standard lowercase", "Char: %c\n", 'a');
	TEST_PRINTF("%c uppercase", "Char: %c\n", 'Z');
	TEST_PRINTF("%c null byte", "Null char: %c\n", '\0');
}

void test_str(void)
{
	printf("\n%s--- Test %%s (Strings) ---%s\n", CYAN, RESET);
	TEST_PRINTF("%s standard string", "Simple: %s\n", "Hello");
	TEST_PRINTF("%s empty string", "Empty: %s\n", "");
	TEST_PRINTF("%s NULL string", "Null: %s\n", (char *)NULL);
}

void test_ptr(void)
{
	printf("\n%s--- Test %%p (Pointers) ---%s\n", CYAN, RESET);
	int x = 42;
	TEST_PRINTF("%p valid memory address", "Pointer: %p\n", (void *)&x);
	TEST_PRINTF("%p NULL pointer address", "Null pointer: %p\n", (void *)NULL);
}

void test_int(void)
{
	printf("\n%s--- Test %%d / %%i (Integers) ---%s\n", CYAN, RESET);
	TEST_PRINTF("%d zero", "Zero: %d\n", 0);
	TEST_PRINTF("%d positive", "Positive: %d\n", 42);
	TEST_PRINTF("%i negative", "Negative: %i\n", -42);
	TEST_PRINTF("%d INT_MAX", "INT_MAX: %d\n", INT_MAX);
	TEST_PRINTF("%i INT_MIN", "INT_MIN: %i\n", INT_MIN);
}

void test_unsigned(void)
{
	printf("\n%s--- Test %%u (Unsigned) ---%s\n", CYAN, RESET);
	TEST_PRINTF("%u zero", "Zero: %u\n", 0);
	TEST_PRINTF("%u positive", "Positive: %u\n", 4242);
	TEST_PRINTF("%u UINT_MAX", "UINT_MAX: %u\n", UINT_MAX);
}

void test_hex(void)
{
	printf("\n%s--- Test %%x / %%X (Hexadecimal) ---%s\n", CYAN, RESET);
	TEST_PRINTF("%x 255 -> ff", "Hex low: %x\n", 255);
	TEST_PRINTF("%X 255 -> FF", "Hex high: %X\n", 255);
	TEST_PRINTF("%x zero", "Hex zero: %x\n", 0);
	TEST_PRINTF("%x 42 -> 2a", "Hex 42: %x\n", 42);
}

void test_percent(void)
{
	printf("\n%s--- Test %%%% (Percent sign) ---%s\n", CYAN, RESET);
	TEST_PRINTF("%% single percent", "Percent: %%\n");
	TEST_PRINTF("%%%%%% multiple percent", "%%%%%%\n");
}

void test_mixed(void)
{
	printf("\n%s--- Test Mixed Conversions ---%s\n", CYAN, RESET);
	TEST_PRINTF("Mixed %s %d %x %%", "User %s has %d coins (hex: %x)%%\n", "Alice", 42, 42);
}

int main(void)
{
	printf("%s=== ft_printf Test Suite ===%s\n", BLUE, RESET);

	test_char();
	test_str();
	test_ptr();
	test_int();
	test_unsigned();
	test_hex();
	test_percent();
	test_mixed();

	printf("\n%s========================================%s\n", BLUE, RESET);
	printf("Tests run: %d | Passed: %s%d%s | Failed: %s%d%s\n",
		g_tests_run,
		GREEN, g_tests_run - g_tests_failed, RESET,
		g_tests_failed > 0 ? RED : GREEN, g_tests_failed, RESET);

	if (g_tests_failed > 0)
	{
		printf("\n%sFailed tests report (%d failure%s):%s\n",
			RED, g_failed_count, g_failed_count > 1 ? "s" : "", RESET);
		for (int i = 0; i < g_failed_count; i++)
		{
			printf("----------------------------------------\n");
			printf("%s[FAIL] %s%s\n", RED, g_failed_tests[i].name, RESET);
			if (g_failed_tests[i].is_int_only)
			{
				printf("  %s-> Expected return : %d%s\n", YELLOW, g_failed_tests[i].exp_ret, RESET);
				printf("  %s-> Got return      : %d%s\n", RED, g_failed_tests[i].got_ret, RESET);
			}
			else
			{
				printf("  %s-> Expected output : %s", YELLOW, RESET);
				print_escaped(g_failed_tests[i].exp_out, g_failed_tests[i].exp_out_len);
				printf(" (len: %d)\n", g_failed_tests[i].exp_out_len);
				printf("  %s-> Expected return : %d%s\n", YELLOW, g_failed_tests[i].exp_ret, RESET);
				printf("  %s-> Got output      : %s", RED, RESET);
				print_escaped(g_failed_tests[i].got_out, g_failed_tests[i].got_out_len);
				printf(" (len: %d)\n", g_failed_tests[i].got_out_len);
				printf("  %s-> Got return      : %d%s\n", RED, g_failed_tests[i].got_ret, RESET);
			}
		}
		printf("----------------------------------------\n");
		return (1);
	}
	printf("%sAll tests passed successfully!%s\n", GREEN, RESET);
	return (0);
}
