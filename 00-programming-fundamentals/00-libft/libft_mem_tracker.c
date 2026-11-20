#include "libft_tests.h"

void	*__real_malloc(size_t size);
void	*__real_calloc(size_t nmemb, size_t size);
void	__real_free(void *ptr);

#define MAX_TRACK_ALLOCS 16384

static void	*g_alloc_ptrs[MAX_TRACK_ALLOCS];
static int	g_alloc_count = 0;

void	*__wrap_malloc(size_t size)
{
	void	*ptr;

	ptr = __real_malloc(size);
	if (ptr && g_alloc_count < MAX_TRACK_ALLOCS)
		g_alloc_ptrs[g_alloc_count++] = ptr;
	return (ptr);
}

void	*__wrap_calloc(size_t nmemb, size_t size)
{
	void	*ptr;

	ptr = __real_calloc(nmemb, size);
	if (ptr && g_alloc_count < MAX_TRACK_ALLOCS)
		g_alloc_ptrs[g_alloc_count++] = ptr;
	return (ptr);
}

void	__wrap_free(void *ptr)
{
	int	i;

	if (ptr)
	{
		i = 0;
		while (i < g_alloc_count)
		{
			if (g_alloc_ptrs[i] == ptr)
			{
				g_alloc_ptrs[i] = g_alloc_ptrs[--g_alloc_count];
				break ;
			}
			i++;
		}
	}
	__real_free(ptr);
}

int	get_active_allocs(void)
{
	return (g_alloc_count);
}
