#include "libft_tests.h"

static void crash_lstadd_back_null_lst(void)
{
	t_list	elem;

	elem.content = "safe";
	elem.next = NULL;
	ft_lstadd_back(NULL, &elem);
}

static void crash_lstadd_back_null_new(void)
{
	t_list	*head;

	head = NULL;
	ft_lstadd_back(&head, NULL);
}

static void crash_lstadd_back_null_new_leaves_unchanged(void)
{
	t_list	*head;
	t_list	n;

	n.content = "node";
	n.next = NULL;
	head = &n;
	ft_lstadd_back(&head, NULL);
	if (head != &n)
		exit(1);
}

void test_lstadd_back(void)
{
	t_list	*head;
	t_list	n1;
	t_list	n2;
	t_list	n3;

	printf("\n--- Testing ft_lstadd_back ---\n");

	head = NULL;
	n1.content = "node 1";
	n1.next = NULL;
	n2.content = "node 2";
	n2.next = NULL;
	n3.content = "node 3";
	n3.next = NULL;

	/* 1. Add to empty list (*lst == NULL) */
	ft_lstadd_back(&head, &n1);
	check_ptr(head, &n1, "ft_lstadd_back sets *lst to new on empty list");
	if (head != NULL)
		check_ptr(head->next, NULL, "head->next remains NULL after adding to empty list");

	/* 2. Add second node to back */
	ft_lstadd_back(&head, &n2);
	check_ptr(head, &n1, "ft_lstadd_back preserves head pointer to node 1");
	if (head != NULL)
	{
		check_ptr(head->next, &n2, "node 1 next points to node 2");
		if (head->next != NULL)
			check_ptr(head->next->next, NULL, "node 2 next is NULL");
	}

	/* 3. Add third node to back */
	ft_lstadd_back(&head, &n3);
	check_ptr(head, &n1, "head remains node 1 after adding node 3");
	if (head != NULL && head->next != NULL)
	{
		check_ptr(head->next, &n2, "node 1 next remains node 2");
		check_ptr(head->next->next, &n3, "node 2 next points to node 3");
		if (head->next->next != NULL)
			check_ptr(head->next->next->next, NULL, "node 3 next is NULL");
	}

	/* 4. Defensive NULL checks */
	check_no_crash(crash_lstadd_back_null_lst, "ft_lstadd_back(NULL, new) handles NULL lst safely (no crash)");
	check_no_crash(crash_lstadd_back_null_new, "ft_lstadd_back(&head, NULL) handles NULL new safely (no crash)");
	check_no_crash(crash_lstadd_back_null_new_leaves_unchanged, "ft_lstadd_back(&head, NULL) leaves list unchanged");
}

/* ========================================================================== */
/*                             ft_lstdelone                                   */
/* ========================================================================== */

