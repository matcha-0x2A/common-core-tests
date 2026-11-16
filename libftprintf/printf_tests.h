#ifndef PRINTF_TESTS_H
# define PRINTF_TESTS_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <limits.h>
# include "ft_printf.h"

# define GREEN   "\033[0;32m"
# define RED     "\033[0;31m"
# define YELLOW  "\033[0;33m"
# define BLUE    "\033[0;34m"
# define CYAN    "\033[0;36m"
# define RESET   "\033[0m"

# define MAX_FAILED_LOG 256

typedef struct s_capture {
	char	buf[4096];
	int		len;
	int		ret;
}	t_capture;

typedef struct s_failed_test {
	char	name[128];
	char	exp_out[512];
	int		exp_out_len;
	int		exp_ret;
	char	got_out[512];
	int		got_out_len;
	int		got_ret;
	int		is_int_only;
}	t_failed_test;

extern t_failed_test	g_failed_tests[MAX_FAILED_LOG];
extern const char		*g_failed_names[MAX_FAILED_LOG];
extern int				g_failed_count;
extern int				g_tests_run;
extern int				g_tests_failed;

void	print_escaped(const char *buf, int len);
void	record_failure(const char *test_name, const t_capture *got, const t_capture *exp);
void	record_failure_simple(const char *test_name);
void	record_failure_int(const char *test_name, int got, int expected);

void	check(int condition, const char *test_name);
void	check_int(int got, int expected, const char *test_name);
void	check_str(const char *got, const char *expected, const char *test_name);

void	capture_call_start(int pfd[2], int *saved_stdout);
void	capture_call_end(int pfd[2], int saved_stdout, t_capture *cap, int ret);
void	compare_and_report(const char *test_name, const t_capture *got, const t_capture *exp);

# define TEST_PRINTF(test_name, ...) \
	do { \
		int (*sys_printf)(const char *, ...) = printf; \
		t_capture got; \
		t_capture exp; \
		int pfd[2], saved; \
		capture_call_start(pfd, &saved); \
		int got_ret = ft_printf(__VA_ARGS__); \
		capture_call_end(pfd, saved, &got, got_ret); \
		capture_call_start(pfd, &saved); \
		int exp_ret = sys_printf(__VA_ARGS__); \
		capture_call_end(pfd, saved, &exp, exp_ret); \
		compare_and_report(test_name, &got, &exp); \
	} while (0)

# define TEST_PRINTF_EXP(test_name, exp_str, exp_ret_val, ...) \
	do { \
		t_capture got; \
		t_capture exp; \
		int pfd[2], saved; \
		capture_call_start(pfd, &saved); \
		int got_ret = ft_printf(__VA_ARGS__); \
		capture_call_end(pfd, saved, &got, got_ret); \
		memset(&exp, 0, sizeof(exp)); \
		size_t l = strlen(exp_str); \
		if (l >= sizeof(exp.buf)) l = sizeof(exp.buf) - 1; \
		memcpy(exp.buf, exp_str, l); \
		exp.buf[l] = '\0'; \
		exp.len = (int)l; \
		exp.ret = exp_ret_val; \
		compare_and_report(test_name, &got, &exp); \
	} while (0)

void	test_char(void);
void	test_str(void);
void	test_ptr(void);
void	test_int(void);
void	test_unsigned(void);
void	test_hex(void);
void	test_percent(void);
void	test_mixed(void);

#endif
