#include "libft_tests.h"

static void crash_lstdelone_null_node(void)
{
	ft_lstdelone(NULL, track_del);
}

static void crash_lstdelone_null_del(void)
{
	t_list *node = malloc(sizeof(t_list));
	if (!node)
		return ;
	node->content = NULL;
	node->next = NULL;
	ft_lstdelone(node, NULL);
}

void test_lstdelone(void)
{
	char	*data;
	t_list	*node;
	char	*s1;
	char	*s2;
	t_list	*n1;
	t_list	*n2;

	printf("\n--- Testing ft_lstdelone ---\n");

	/* 1. Basic deletion: calls del exactly once and passes node->content */
	data = strdup("42 network");
	node = ft_lstnew(data);
	g_del_count = 0;
	g_last_del_ptr = NULL;
	ft_lstdelone(node, track_del);
	check_int(g_del_count, 1, "ft_lstdelone calls del exactly once");
	check_ptr(g_last_del_ptr, data, "ft_lstdelone passes node->content pointer to del");
	free(data);

	/* 2. Real heap deletion using free as del */
	data = strdup("dynamically allocated string");
	node = ft_lstnew(data);
	g_custom_freed = 0;
	ft_lstdelone(node, custom_free_del);
	check_int(g_custom_freed, 1, "ft_lstdelone with custom_free frees content");
	if (g_custom_freed == 0)
	{
		free(data);
		free(node);
	}

	/* 3. Node in a chain: next pointer must NOT be freed */
	s1 = strdup("chain node 1");
	s2 = strdup("chain node 2");
	n1 = ft_lstnew(s1);
	n2 = ft_lstnew(s2);
	if (n1 && n2)
	{
		n1->next = n2;
		g_del_count = 0;
		g_last_del_ptr = NULL;
		ft_lstdelone(n1, track_del);
		check_int(g_del_count, 1, "ft_lstdelone on n1 calls del once");
		check_ptr(g_last_del_ptr, s1, "ft_lstdelone on n1 only deleted s1");
		check_str((char *)n2->content, "chain node 2", "ft_lstdelone on n1 leaves n2 content intact");
		check_ptr(n2->next, NULL, "n2 next remains NULL");
		free(s1);
		free(s2);
		free(n2);
	}
	else
	{
		if (n1)
			free(n1);
		if (n2)
			free(n2);
		free(s1);
		free(s2);
	}

	/* 4. Node with NULL content */
	node = ft_lstnew(NULL);
	g_del_count = 0;
	g_last_del_ptr = (void *)0x1;
	ft_lstdelone(node, track_del);
	check_int(g_del_count, 1, "ft_lstdelone with NULL content still calls del");
	check_ptr(g_last_del_ptr, NULL, "del received NULL content");

	/* 5. Defensive checks (NULL node or NULL del) */
	check_no_crash(crash_lstdelone_null_node, "ft_lstdelone(NULL, del) handles NULL node safely (no crash)");
	check_no_crash(crash_lstdelone_null_del, "ft_lstdelone(node, NULL) handles NULL del safely (no crash)");
}

/* ========================================================================== */
/*                              ft_lstclear                                   */
/* ========================================================================== */

