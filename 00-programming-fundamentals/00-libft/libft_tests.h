#ifndef LIBFT_TESTS_H
# define LIBFT_TESTS_H

# include <stdio.h>
# include <stdint.h>
# include <stdlib.h>
# include <string.h>
# include <strings.h>
# include <ctype.h>
# include <assert.h>
# include <sys/wait.h>
# include <signal.h>
# include <unistd.h>
# include <limits.h>
# include "libft.h"

# define GREEN "\033[0;32m"
# define RED   "\033[0;31m"
# define YELLOW "\033[0;33m"
# define BLUE  "\033[0;34m"
# define CYAN  "\033[0;36m"
# define RESET "\033[0m"

# define MAX_FAILED_LOG 256
extern const char *g_failed_names[MAX_FAILED_LOG];
extern int g_failed_count;
extern int g_tests_run;
extern int g_tests_failed;

/* Shared list test utilities */
extern int g_del_count;
extern void *g_last_del_ptr;
extern int g_custom_freed;
extern int g_del_order[16];
extern int g_del_order_idx;

void track_del(void *content);
void custom_free_del(void *content);
void order_del(void *content);

/* Framework assertions */
void record_failure(const char *test_name);
void check(int condition, const char *test_name);
void check_int(int got, int expected, const char *test_name);
void check_uint(size_t got, size_t expected, const char *test_name);
void check_str(const char *got, const char *expected, const char *test_name);
void check_ptr(const void *got, const void *expected, const char *test_name);
void check_cmp(int got, int expected, const char *test_name);
void check_crash(void (*func)(void), const char *test_name);
void check_no_crash(void (*func)(void), const char *test_name);
void check_malloc_nth_protection(void *(*func)(void), int nth, const char *test_name);
void check_malloc_protection(void *(*func)(void), const char *test_name);
int  get_active_allocs(void);
void set_malloc_failure(int nth);
void reset_malloc_failure(void);
void set_malloc_dirty(int enable);

/* Test prototypes */
void test_isalpha(void);
void test_isdigit(void);
void test_isalnum(void);
void test_isascii(void);
void test_isprint(void);
void test_strlen(void);
void test_toupper(void);
void test_tolower(void);
void test_memchr(void);
void test_memset(void);
void test_bzero(void);
void test_memcpy(void);
void test_memmove(void);
void test_calloc(void);
void test_strdup(void);
void test_substr(void);
void test_strjoin(void);
void test_strtrim(void);
void test_split(void);
void test_itoa(void);
void test_strmapi(void);
void test_striteri(void);
void test_putnbr_fd(void);
void test_strchr(void);
void test_strrchr(void);
void test_strlcpy(void);
void test_strlcat(void);
void test_strncmp(void);
void test_memcmp(void);
void test_atoi(void);
void test_strnstr(void);
void test_lstnew(void);
void test_lstadd_front(void);
void test_lstsize(void);
void test_lstlast(void);
void test_lstadd_back(void);
void test_lstdelone(void);
void test_lstclear(void);
void test_lstiter(void);
void test_lstmap(void);
void test_lst_lifecycle(void);

#endif
