#include "libft_tests.h"

static void	check_putnbr_case(int n, const char *expected, const char *desc)
{
	int		pfd[2];
	char	buf[64];
	ssize_t	bytes_read;
	int		before;
	int		after;

	if (pipe(pfd) == -1)
	{
		record_failure(desc);
		return ;
	}
	before = get_active_allocs();
	ft_putnbr_fd(n, pfd[1]);
	after = get_active_allocs();
	close(pfd[1]);
	memset(buf, 0, sizeof(buf));
	bytes_read = read(pfd[0], buf, sizeof(buf) - 1);
	close(pfd[0]);
	if (bytes_read < 0)
		buf[0] = '\0';
	else
		buf[bytes_read] = '\0';
	check_str(buf, expected, desc);
	check(after == before, "ft_putnbr_fd does not leak memory");
}

static void	crash_putnbr_invalid_fd(void)
{
	ft_putnbr_fd(42, -1);
}

static void	crash_putnbr_invalid_fd_zero(void)
{
	ft_putnbr_fd(0, -42);
}

void test_putnbr_fd(void)
{
	printf("\n--- Testing ft_putnbr_fd ---\n");

	/* 1. Zero */
	check_putnbr_case(0, "0", "ft_putnbr_fd(0) outputs \"0\"");

	/* 2. Positive single digits */
	check_putnbr_case(1, "1", "ft_putnbr_fd(1) outputs \"1\"");
	check_putnbr_case(5, "5", "ft_putnbr_fd(5) outputs \"5\"");
	check_putnbr_case(9, "9", "ft_putnbr_fd(9) outputs \"9\"");

	/* 3. Negative single digits */
	check_putnbr_case(-1, "-1", "ft_putnbr_fd(-1) outputs \"-1\"");
	check_putnbr_case(-5, "-5", "ft_putnbr_fd(-5) outputs \"-5\"");
	check_putnbr_case(-9, "-9", "ft_putnbr_fd(-9) outputs \"-9\"");

	/* 4. Standard positive numbers */
	check_putnbr_case(42, "42", "ft_putnbr_fd(42) outputs \"42\"");
	check_putnbr_case(123, "123", "ft_putnbr_fd(123) outputs \"123\"");
	check_putnbr_case(987654, "987654", "ft_putnbr_fd(987654) outputs \"987654\"");

	/* 5. Standard negative numbers */
	check_putnbr_case(-42, "-42", "ft_putnbr_fd(-42) outputs \"-42\"");
	check_putnbr_case(-123, "-123", "ft_putnbr_fd(-123) outputs \"-123\"");
	check_putnbr_case(-987654, "-987654", "ft_putnbr_fd(-987654) outputs \"-987654\"");

	/* 6. Powers and multiples of 10 */
	check_putnbr_case(10, "10", "ft_putnbr_fd(10) outputs \"10\"");
	check_putnbr_case(100, "100", "ft_putnbr_fd(100) outputs \"100\"");
	check_putnbr_case(1000000, "1000000", "ft_putnbr_fd(1000000) outputs \"1000000\"");
	check_putnbr_case(-10, "-10", "ft_putnbr_fd(-10) outputs \"-10\"");
	check_putnbr_case(-100, "-100", "ft_putnbr_fd(-100) outputs \"-100\"");
	check_putnbr_case(-1000000, "-1000000", "ft_putnbr_fd(-1000000) outputs \"-1000000\"");

	/* 7. Limits (INT_MAX & INT_MIN) */
	check_putnbr_case(2147483647, "2147483647", "ft_putnbr_fd(INT_MAX) outputs \"2147483647\"");
	check_putnbr_case(2147483646, "2147483646", "ft_putnbr_fd(INT_MAX - 1) outputs \"2147483646\"");
	check_putnbr_case(-2147483648, "-2147483648", "ft_putnbr_fd(INT_MIN) outputs \"-2147483648\"");
	check_putnbr_case(-2147483647, "-2147483647", "ft_putnbr_fd(INT_MIN + 1) outputs \"-2147483647\"");

	/* 8. Invalid file descriptor safety */
	check_no_crash(crash_putnbr_invalid_fd, "ft_putnbr_fd(42, -1) handles invalid negative fd safely (no crash)");
	check_no_crash(crash_putnbr_invalid_fd_zero, "ft_putnbr_fd(0, -42) handles invalid negative fd safely (no crash)");
}
