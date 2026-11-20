#include "libft_tests.h"

static void crash_memset_null(void)
{
	ft_memset(NULL, 'A', 5);
}

void test_memset(void)
{
	char	buf1[32];
	char	buf2[32];
	char	guard_buf[16];
	size_t	i;
	int		match;
	char	large1[1024];
	char	large2[1024];

	printf("\n--- Testing ft_memset ---\n");

	/* 1. Return value: must return original pointer s */
	check(ft_memset(buf1, 'A', 10) == buf1, "ft_memset returns s pointer");

	/* 2. Basic fill */
	ft_memset(buf1, 'B', 10);
	match = 1;
	for (i = 0; i < 10; i++)
	{
		if (buf1[i] != 'B')
			match = 0;
	}
	check(match == 1, "ft_memset fills entire range (10 bytes with 'B')");

	/* 3. Partial fill & boundaries (must not write out of bounds) */
	memset(guard_buf, 'X', sizeof(guard_buf));
	ft_memset(guard_buf + 3, 'Y', 5);
	check(guard_buf[0] == 'X' && guard_buf[1] == 'X' && guard_buf[2] == 'X',
		"ft_memset respects start boundary (prefix untouched)");
	check(guard_buf[3] == 'Y' && guard_buf[4] == 'Y' && guard_buf[5] == 'Y'
		&& guard_buf[6] == 'Y' && guard_buf[7] == 'Y',
		"ft_memset fills expected middle slice");
	check(guard_buf[8] == 'X' && guard_buf[9] == 'X' && guard_buf[15] == 'X',
		"ft_memset respects end boundary (suffix untouched)");

	/* 4. Fill with zero / NUL byte */
	memset(buf1, 0xFF, sizeof(buf1));
	ft_memset(buf1, 0, 16);
	match = 1;
	for (i = 0; i < 16; i++)
	{
		if (buf1[i] != 0)
			match = 0;
	}
	check(match == 1 && (unsigned char)buf1[16] == 0xFF, "ft_memset fills with 0 (and stops at n)");

	/* 5. n == 0 (must do nothing and return s) */
	memset(buf1, 'K', sizeof(buf1));
	check(ft_memset(buf1, 'Z', 0) == buf1, "ft_memset with n = 0 returns s");
	match = 1;
	for (i = 0; i < sizeof(buf1); i++)
	{
		if (buf1[i] != 'K')
			match = 0;
	}
	check(match == 1, "ft_memset with n = 0 does not modify memory");

	/* 6. Unsigned char conversions (> 127, negative, c > 255) */
	ft_memset(buf1, 128, 5);
	check((unsigned char)buf1[0] == 128 && (unsigned char)buf1[4] == 128,
		"ft_memset fills with c = 128 (0x80)");

	ft_memset(buf1, -1, 5);
	check((unsigned char)buf1[0] == 255 && (unsigned char)buf1[4] == 255,
		"ft_memset fills with c = -1 (converted to 0xFF)");

	ft_memset(buf1, -42, 5);
	check((unsigned char)buf1[0] == (unsigned char)-42,
		"ft_memset fills with negative value (converted to unsigned char)");

	ft_memset(buf1, 'A' + 256, 5);
	check(buf1[0] == 'A' && buf1[4] == 'A',
		"ft_memset fills with c > 255 (truncated to unsigned char)");

	ft_memset(buf1, 0x12345600 | 'Q', 5);
	check(buf1[0] == 'Q' && buf1[4] == 'Q',
		"ft_memset fills with multi-byte int (truncated to unsigned char)");

	/* 7. Comparison with libc memset */
	memset(buf1, 0xAA, sizeof(buf1));
	memset(buf2, 0xAA, sizeof(buf2));
	ft_memset(buf1 + 2, 'C', 15);
	memset(buf2 + 2, 'C', 15);
	check(memcmp(buf1, buf2, sizeof(buf1)) == 0, "ft_memset matches libc memset (slice)");

	memset(buf1, 0xAA, sizeof(buf1));
	memset(buf2, 0xAA, sizeof(buf2));
	ft_memset(buf1, -120, 20);
	memset(buf2, -120, 20);
	check(memcmp(buf1, buf2, sizeof(buf1)) == 0, "ft_memset matches libc memset (negative c)");

	/* 8. Large buffer */
	ft_memset(large1, 0x42, sizeof(large1));
	memset(large2, 0x42, sizeof(large2));
	check(memcmp(large1, large2, sizeof(large1)) == 0, "ft_memset matches libc memset on 1024 bytes");

	/* 9. NULL pointer handling (crash attendu si n > 0, pas de crash si n = 0) */
	check(ft_memset(NULL, 'A', 0) == NULL, "ft_memset(NULL, 'A', 0) == NULL (n = 0, no crash)");
	check_crash(crash_memset_null, "ft_memset(NULL, 'A', 5) raises SIGSEGV (crash attendu)");
}

