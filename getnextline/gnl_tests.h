#ifndef GNL_TESTS_H
# define GNL_TESTS_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <fcntl.h>
# include "get_next_line.h"

# define GREEN   "\033[0;32m"
# define RED     "\033[0;31m"
# define YELLOW  "\033[0;33m"
# define BLUE    "\033[0;34m"
# define CYAN    "\033[0;36m"
# define RESET   "\033[0m"

# define MAX_FAILED_LOG 256
extern const char *g_failed_names[MAX_FAILED_LOG];
extern int g_failed_count;
extern int g_tests_run;
extern int g_tests_failed;

void check(int condition, const char *test_name);
void check_str(const char *got, const char *expected, const char *test_name);
void check_ptr(const void *got, const void *expected, const char *test_name);

void test_invalid_fd(void);
void test_empty_file(void);
void test_single_line_with_nl(void);
void test_single_line_no_nl(void);
void test_multiple_lines(void);
void test_consecutive_newlines(void);

#endif
