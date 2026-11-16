#include "libft_tests.h"

static int g_iter_count = 0;
static void *g_last_iter_ptr = NULL;
static int g_iter_history[16];
static int g_iter_hist_idx = 0;

static void track_iter(void *content)
{
	g_iter_count++;
	g_last_iter_ptr = content;
}

static void iter_inc(void *content)
{
	if (content)
		(*(int *)content) += 1;
	g_iter_count++;
}

static void iter_record(void *content)
{
	if (g_iter_hist_idx < 16)
		g_iter_history[g_iter_hist_idx++] = (int)(intptr_t)content;
	g_iter_count++;
}

static void iter_toupper_str(void *content)
{
	char *str = (char *)content;

	if (!str)
		return ;
	while (*str)
	{
		if (*str >= 'a' && *str <= 'z')
			*str -= 32;
		str++;
	}
	g_iter_count++;
}

static void crash_lstiter_null_lst(void)
{
	ft_lstiter(NULL, iter_inc);
}

static void crash_lstiter_null_f(void)
{
	t_list node;

	node.content = NULL;
	node.next = NULL;
	ft_lstiter(&node, NULL);
}

void test_lstiter(void)
{
	t_list	*head;
	t_list	*n1;
	t_list	*n2;
	t_list	*n3;
	int		val1;
	int		val2;
	int		val3;
	char	s1[16];
	char	s2[16];
	int		arr[100];
	int		i;
	int		order_ok;

	printf("\n--- Testing ft_lstiter ---\n");

	/* 1. Empty list (NULL) */
	g_iter_count = 0;
	ft_lstiter(NULL, iter_inc);
	check_int(g_iter_count, 0, "ft_lstiter(NULL, f) calls f 0 times");

	/* 2. Single node: in-place modification */
	val1 = 41;
	head = ft_lstnew(&val1);
	g_iter_count = 0;
	ft_lstiter(head, iter_inc);
	check_int(g_iter_count, 1, "ft_lstiter calls f exactly once on single node");
	check_int(val1, 42, "ft_lstiter applied function modifying single node content (41 -> 42)");
	free(head);

	/* 3. Multi-node list: in-place modification on 3 nodes */
	val1 = 10;
	val2 = 20;
	val3 = 30;
	head = NULL;
	ft_lstadd_back(&head, ft_lstnew(&val1));
	ft_lstadd_back(&head, ft_lstnew(&val2));
	ft_lstadd_back(&head, ft_lstnew(&val3));
	g_iter_count = 0;
	ft_lstiter(head, iter_inc);
	check_int(g_iter_count, 3, "ft_lstiter calls f 3 times for 3 nodes");
	check_int(val1, 11, "ft_lstiter modified node 1 content (10 -> 11)");
	check_int(val2, 21, "ft_lstiter modified node 2 content (20 -> 21)");
	check_int(val3, 31, "ft_lstiter modified node 3 content (30 -> 31)");
	ft_lstclear(&head, track_del);

	/* 4. String content modification in place */
	strcpy(s1, "hello");
	strcpy(s2, "world");
	head = NULL;
	ft_lstadd_back(&head, ft_lstnew(s1));
	ft_lstadd_back(&head, ft_lstnew(s2));
	g_iter_count = 0;
	ft_lstiter(head, iter_toupper_str);
	check_int(g_iter_count, 2, "ft_lstiter calls toupper on 2 string nodes");
	check_str(s1, "HELLO", "ft_lstiter uppercase applied to first string (\"hello\" -> \"HELLO\")");
	check_str(s2, "WORLD", "ft_lstiter uppercase applied to second string (\"world\" -> \"WORLD\")");
	ft_lstclear(&head, track_del);

	/* 5. Sequential order of iteration (first to last) */
	head = NULL;
	ft_lstadd_back(&head, ft_lstnew((void *)(intptr_t)100));
	ft_lstadd_back(&head, ft_lstnew((void *)(intptr_t)200));
	ft_lstadd_back(&head, ft_lstnew((void *)(intptr_t)300));
	g_iter_hist_idx = 0;
	g_iter_count = 0;
	memset(g_iter_history, 0, sizeof(g_iter_history));
	ft_lstiter(head, iter_record);
	check_int(g_iter_count, 3, "ft_lstiter called record 3 times");
	order_ok = (g_iter_hist_idx == 3 && g_iter_history[0] == 100 && g_iter_history[1] == 200 && g_iter_history[2] == 300);
	check(order_ok, "ft_lstiter visits nodes in exact sequential order (100 -> 200 -> 300)");
	ft_lstclear(&head, track_del);

	/* 6. List structure preservation (next pointers unchanged) */
	n1 = ft_lstnew("a");
	n2 = ft_lstnew("b");
	n3 = ft_lstnew("c");
	n1->next = n2;
	n2->next = n3;
	n3->next = NULL;
	g_iter_count = 0;
	ft_lstiter(n1, track_del);
	check(n1->next == n2 && n2->next == n3 && n3->next == NULL,
		"ft_lstiter preserves list structure and next pointers unmodified");
	free(n1);
	free(n2);
	free(n3);

	/* 7. Large list iteration (100 nodes) */
	head = NULL;
	for (i = 0; i < 100; i++)
	{
		arr[i] = i;
		ft_lstadd_back(&head, ft_lstnew(&arr[i]));
	}
	check_uint(ft_lstsize(head), 100, "Created list with 100 nodes for iter");
	g_iter_count = 0;
	ft_lstiter(head, iter_inc);
	check_int(g_iter_count, 100, "ft_lstiter visited all 100 nodes");
	check_int(arr[0], 1, "First element incremented (0 -> 1)");
	check_int(arr[50], 51, "Middle element incremented (50 -> 51)");
	check_int(arr[99], 100, "Last element incremented (99 -> 100)");
	ft_lstclear(&head, track_del);

	/* 8. Node with NULL content */
	head = ft_lstnew(NULL);
	g_iter_count = 0;
	g_last_iter_ptr = (void *)0x1;
	ft_lstiter(head, track_iter);
	check_int(g_iter_count, 1, "ft_lstiter calls f on node with NULL content");
	check_ptr(g_last_iter_ptr, NULL, "f received NULL content pointer");
	free(head);

	/* 9. Defensive NULL checks */
	check_no_crash(crash_lstiter_null_lst, "ft_lstiter(NULL, f) handles NULL lst safely (no crash)");
	check_no_crash(crash_lstiter_null_f, "ft_lstiter(lst, NULL) handles NULL f safely (no crash)");
}

/* ========================================================================== */
/*                              ft_lstmap                                     */
/* ========================================================================== */

