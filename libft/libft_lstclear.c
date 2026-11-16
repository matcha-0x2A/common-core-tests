#include "libft_tests.h"

static void crash_lstclear_null_lst(void)
{
	ft_lstclear(NULL, track_del);
}

static void crash_lstclear_null_head(void)
{
	t_list	*head;

	head = NULL;
	ft_lstclear(&head, track_del);
}

static void crash_lstclear_null_del(void)
{
	t_list *node = malloc(sizeof(t_list));
	if (!node)
		return ;
	node->content = NULL;
	node->next = NULL;
	ft_lstclear(&node, NULL);
}

void test_lstclear(void)
{
	t_list	*head;
	t_list	*n1;
	t_list	*n2;
	t_list	*n3;
	char	*data;
	char	*s1;
	char	*s2;
	char	*s3;
	int		i;
	int		order_ok;

	printf("\n--- Testing ft_lstclear ---\n");

	/* 1. Empty list (*lst == NULL) */
	head = NULL;
	g_del_count = 0;
	ft_lstclear(&head, track_del);
	check_ptr(head, NULL, "ft_lstclear on empty list leaves *lst as NULL");
	check_int(g_del_count, 0, "ft_lstclear on empty list calls del 0 times");

	/* 2. Single node list */
	data = strdup("only one");
	head = ft_lstnew(data);
	g_custom_freed = 0;
	ft_lstclear(&head, custom_free_del);
	check_ptr(head, NULL, "ft_lstclear sets *lst to NULL on single node");
	check_int(g_custom_freed, 1, "ft_lstclear calls del once on single node list");
	if (g_custom_freed == 0)
	{
		free(data);
		free(head);
	}

	/* 3. Multi-node list (5 nodes) */
	head = NULL;
	for (i = 0; i < 5; i++)
	{
		data = strdup("node data");
		ft_lstadd_back(&head, ft_lstnew(data));
	}
	check_uint(ft_lstsize(head), 5, "List initialized with 5 nodes");
	g_custom_freed = 0;
	ft_lstclear(&head, custom_free_del);
	check_ptr(head, NULL, "ft_lstclear sets head to NULL after clearing 5 nodes");
	check_int(g_custom_freed, 5, "ft_lstclear calls del exactly 5 times for 5 nodes");
	if (g_custom_freed < 5)
	{
		t_list *curr = head;
		while (curr)
		{
			t_list *next = curr->next;
			free(curr->content);
			free(curr);
			curr = next;
		}
	}

	/* 4. Deletion order (must be sequential from first to last: 10, 20, 30) */
	head = NULL;
	ft_lstadd_back(&head, ft_lstnew((void *)(intptr_t)10));
	ft_lstadd_back(&head, ft_lstnew((void *)(intptr_t)20));
	ft_lstadd_back(&head, ft_lstnew((void *)(intptr_t)30));
	g_del_order_idx = 0;
	memset(g_del_order, 0, sizeof(g_del_order));
	ft_lstclear(&head, order_del);
	check_ptr(head, NULL, "ft_lstclear sets head to NULL for ordered list");
	order_ok = (g_del_order_idx == 3 && g_del_order[0] == 10 && g_del_order[1] == 20 && g_del_order[2] == 30);
	check(order_ok, "ft_lstclear deletes elements in sequential order from front to back");
	if (head != NULL)
	{
		t_list *curr = head;
		while (curr)
		{
			t_list *next = curr->next;
			free(curr);
			curr = next;
		}
	}

	/* 5. Sublist clearing: clearing &n2 leaves n1 intact */
	s1 = strdup("first");
	s2 = strdup("second");
	s3 = strdup("third");
	n1 = ft_lstnew(s1);
	n2 = ft_lstnew(s2);
	n3 = ft_lstnew(s3);
	if (n1 && n2 && n3)
	{
		n1->next = n2;
		n2->next = n3;
		g_custom_freed = 0;
		ft_lstclear(&n2, custom_free_del);
		check_ptr(n2, NULL, "ft_lstclear sets sublist head (n2) to NULL");
		check_int(g_custom_freed, 2, "ft_lstclear freed both sublist nodes (n2 and n3)");
		check_str((char *)n1->content, "first", "n1 content remained intact");
		n1->next = NULL;
		ft_lstdelone(n1, custom_free_del);
		if (n2 != NULL)
		{
			free(s2);
			free(s3);
			free(n2);
			free(n3);
		}
	}

	/* 6. Large list (100 nodes) */
	head = NULL;
	for (i = 0; i < 100; i++)
		ft_lstadd_back(&head, ft_lstnew(NULL));
	check_uint(ft_lstsize(head), 100, "Created list with 100 nodes");
	g_del_count = 0;
	ft_lstclear(&head, track_del);
	check_ptr(head, NULL, "ft_lstclear sets head to NULL after clearing 100 nodes");
	check_int(g_del_count, 100, "ft_lstclear calls del 100 times for 100 nodes");
	if (head != NULL)
	{
		t_list *curr = head;
		while (curr)
		{
			t_list *next = curr->next;
			free(curr);
			curr = next;
		}
	}

	/* 7. Defensive NULL checks */
	check_no_crash(crash_lstclear_null_lst, "ft_lstclear(NULL, del) handles NULL lst safely (no crash)");
	check_no_crash(crash_lstclear_null_head, "ft_lstclear(&head_null, del) handles empty head safely (no crash)");
	check_no_crash(crash_lstclear_null_del, "ft_lstclear(&head, NULL) handles NULL del safely (no crash)");
}

/* ========================================================================== */
/*                              ft_lstiter                                    */
/* ========================================================================== */

