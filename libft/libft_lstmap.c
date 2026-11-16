#include "libft_tests.h"

static int g_map_f_count = 0;
static int g_map_del_count = 0;
static int g_map_history[16];
static int g_map_hist_idx = 0;

static void *map_strdup_upper(void *content)
{
	char	*src;
	char	*dup;
	int		i;

	src = (char *)content;
	g_map_f_count++;
	if (!src)
		return (NULL);
	dup = strdup(src);
	if (!dup)
		return (NULL);
	for (i = 0; dup[i]; i++)
	{
		if (dup[i] >= 'a' && dup[i] <= 'z')
			dup[i] -= 32;
	}
	return (dup);
}

static void *map_record_and_mul2(void *content)
{
	int	val;

	val = (int)(intptr_t)content;
	if (g_map_hist_idx < 16)
		g_map_history[g_map_hist_idx++] = val;
	g_map_f_count++;
	return ((void *)(intptr_t)(val * 2));
}

static void *map_add_500(void *content)
{
	int	val;

	val = (int)(intptr_t)content;
	g_map_f_count++;
	return ((void *)(intptr_t)(val + 500));
}

static void *map_null_to_str(void *content)
{
	g_map_f_count++;
	if (content == NULL)
		return (strdup("was_null"));
	return (strdup((char *)content));
}

static void map_track_del(void *content)
{
	g_map_del_count++;
	free(content);
}

static void map_dummy_del(void *content)
{
	(void)content;
	g_map_del_count++;
}

static void crash_lstmap_null_lst(void)
{
	ft_lstmap(NULL, map_strdup_upper, free);
}

static void crash_lstmap_null_f(void)
{
	t_list	node;

	node.content = NULL;
	node.next = NULL;
	ft_lstmap(&node, NULL, free);
}

static void crash_lstmap_null_del(void)
{
	t_list	node;

	node.content = NULL;
	node.next = NULL;
	ft_lstmap(&node, map_strdup_upper, NULL);
}

static void crash_lstmap_all_null(void)
{
	ft_lstmap(NULL, NULL, NULL);
}

void test_lstmap(void)
{
	t_list	*head;
	t_list	*res;
	int		i;
	int		order_ok;
	int		all_correct;

	printf("\n--- Testing ft_lstmap ---\n");

	/* 1. NULL list parameter */
	g_map_f_count = 0;
	g_map_del_count = 0;
	res = ft_lstmap(NULL, map_strdup_upper, free);
	check_ptr(res, NULL, "ft_lstmap(NULL, f, del) returns NULL");
	check_int(g_map_f_count, 0, "ft_lstmap(NULL, f, del) does not call f");
	check_int(g_map_del_count, 0, "ft_lstmap(NULL, f, del) does not call del");

	/* 2. Single node mapping */
	head = ft_lstnew(strdup("hello"));
	g_map_f_count = 0;
	g_map_del_count = 0;
	res = ft_lstmap(head, map_strdup_upper, map_track_del);
	check(res != NULL, "ft_lstmap returns non-NULL for single node");
	check(res != head, "ft_lstmap returns a new node, not original node pointer");
	if (res)
	{
		check_uint(ft_lstsize(res), 1, "Mapped list has size 1");
		check_str((char *)res->content, "HELLO", "Content transformed correctly (\"hello\" -> \"HELLO\")");
		check_ptr(res->next, NULL, "Single mapped node has next == NULL");
		check(res->content != head->content, "Mapped node content is a new allocation, not original pointer");
	}
	check_str((char *)head->content, "hello", "Original node content remained untouched (\"hello\")");
	check_ptr(head->next, NULL, "Original node next remained untouched (NULL)");
	check_int(g_map_f_count, 1, "f called exactly once for single node");
	check_int(g_map_del_count, 0, "del is not called during successful mapping");
	ft_lstclear(&head, free);
	ft_lstclear(&res, free);

	/* 3. Multi-node mapping (3 nodes) */
	head = NULL;
	ft_lstadd_back(&head, ft_lstnew(strdup("alpha")));
	ft_lstadd_back(&head, ft_lstnew(strdup("beta")));
	ft_lstadd_back(&head, ft_lstnew(strdup("gamma")));
	g_map_f_count = 0;
	g_map_del_count = 0;
	res = ft_lstmap(head, map_strdup_upper, map_track_del);
	check(res != NULL, "ft_lstmap returns non-NULL for 3 nodes");
	if (res)
	{
		check_uint(ft_lstsize(res), 3, "Mapped list has size 3");
		check_str((char *)res->content, "ALPHA", "Node 1 transformed correctly (\"ALPHA\")");
		if (res->next)
		{
			check_str((char *)res->next->content, "BETA", "Node 2 transformed correctly (\"BETA\")");
			if (res->next->next)
			{
				check_str((char *)res->next->next->content, "GAMMA", "Node 3 transformed correctly (\"GAMMA\")");
				check_ptr(res->next->next->next, NULL, "Last node next is NULL");
			}
			else
				check(0, "Node 3 exists");
		}
		else
			check(0, "Node 2 exists");

		/* Verify node pointers are all newly allocated */
		check(res != head, "New list head is distinct from original head");
		if (head->next && res->next)
			check(res->next != head->next, "New node 2 is distinct from original node 2");
		if (head->next && head->next->next && res->next && res->next->next)
			check(res->next->next != head->next->next, "New node 3 is distinct from original node 3");
	}
	/* Verify original list is completely intact */
	check_uint(ft_lstsize(head), 3, "Original list still has size 3");
	check_str((char *)head->content, "alpha", "Original node 1 untouched");
	if (head->next)
		check_str((char *)head->next->content, "beta", "Original node 2 untouched");
	if (head->next && head->next->next)
		check_str((char *)head->next->next->content, "gamma", "Original node 3 untouched");
	check_int(g_map_f_count, 3, "f called exactly 3 times");
	check_int(g_map_del_count, 0, "del called 0 times during successful 3-node mapping");
	ft_lstclear(&head, free);
	ft_lstclear(&res, free);

	/* 4. Sequential application order */
	head = NULL;
	ft_lstadd_back(&head, ft_lstnew((void *)(intptr_t)10));
	ft_lstadd_back(&head, ft_lstnew((void *)(intptr_t)20));
	ft_lstadd_back(&head, ft_lstnew((void *)(intptr_t)30));
	g_map_f_count = 0;
	g_map_hist_idx = 0;
	memset(g_map_history, 0, sizeof(g_map_history));
	res = ft_lstmap(head, map_record_and_mul2, map_dummy_del);
	order_ok = (g_map_hist_idx == 3 && g_map_history[0] == 10 && g_map_history[1] == 20 && g_map_history[2] == 30);
	check(order_ok, "ft_lstmap applies f in sequential order (10 -> 20 -> 30)");
	if (res)
	{
		int v1 = (int)(intptr_t)res->content;
		int v2 = res->next ? (int)(intptr_t)res->next->content : 0;
		int v3 = (res->next && res->next->next) ? (int)(intptr_t)res->next->next->content : 0;
		check(v1 == 20 && v2 == 40 && v3 == 60, "Mapped list preserves values in order (20 -> 40 -> 60)");
	}
	else
		check(0, "Mapped list exists for sequential test");
	ft_lstclear(&head, map_dummy_del);
	ft_lstclear(&res, map_dummy_del);

	/* 5. Memory independence: freeing original list leaves mapped list unaffected */
	head = NULL;
	ft_lstadd_back(&head, ft_lstnew(strdup("one")));
	ft_lstadd_back(&head, ft_lstnew(strdup("two")));
	ft_lstadd_back(&head, ft_lstnew(strdup("three")));
	res = ft_lstmap(head, map_strdup_upper, free);
	/* Destroy original list completely */
	ft_lstclear(&head, free);
	check_ptr(head, NULL, "Original list cleared to NULL");
	/* Check res is still fully readable and valid */
	check(res != NULL, "Mapped list still exists after clearing original list");
	if (res)
	{
		check_uint(ft_lstsize(res), 3, "Mapped list size is still 3 after clearing original list");
		check_str((char *)res->content, "ONE", "Mapped node 1 content valid after clearing original");
		if (res->next)
			check_str((char *)res->next->content, "TWO", "Mapped node 2 content valid after clearing original");
		if (res->next && res->next->next)
			check_str((char *)res->next->next->content, "THREE", "Mapped node 3 content valid after clearing original");
	}
	ft_lstclear(&res, free);

	/* 6. Node with NULL content */
	head = ft_lstnew(NULL);
	g_map_f_count = 0;
	res = ft_lstmap(head, map_null_to_str, free);
	check(res != NULL, "ft_lstmap handles node with NULL content");
	if (res)
	{
		check_str((char *)res->content, "was_null", "f called on NULL content returned valid transformed content");
	}
	free(head);
	ft_lstclear(&res, free);

	/* 7. Large list (100 nodes) */
	head = NULL;
	for (i = 0; i < 100; i++)
		ft_lstadd_back(&head, ft_lstnew((void *)(intptr_t)i));
	check_uint(ft_lstsize(head), 100, "Created 100 nodes for lstmap");
	g_map_f_count = 0;
	res = ft_lstmap(head, map_add_500, map_dummy_del);
	check(res != NULL, "ft_lstmap succeeds on 100-node list");
	if (res)
	{
		t_list *curr = res;
		check_uint(ft_lstsize(res), 100, "Mapped list has 100 nodes");
		all_correct = 1;
		for (i = 0; i < 100 && curr; i++)
		{
			if ((int)(intptr_t)curr->content != i + 500)
				all_correct = 0;
			curr = curr->next;
		}
		check(all_correct && i == 100, "All 100 nodes correctly mapped (0..99 -> 500..599)");
	}
	ft_lstclear(&head, map_dummy_del);
	ft_lstclear(&res, map_dummy_del);

	/* 8. Defensive NULL checks */
	check_no_crash(crash_lstmap_null_lst, "ft_lstmap(NULL, f, del) handles NULL lst safely (no crash)");
	check_no_crash(crash_lstmap_null_f, "ft_lstmap(head, NULL, del) handles NULL f safely (no crash)");
	check_no_crash(crash_lstmap_null_del, "ft_lstmap(head, f, NULL) handles NULL del safely (no crash)");
	check_no_crash(crash_lstmap_all_null, "ft_lstmap(NULL, NULL, NULL) handles all NULL safely (no crash)");
}

