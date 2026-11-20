#include "libft_tests.h"

static void crash_lstadd_front_null_lst(void)
{
	t_list	elem;

	elem.content = "safe";
	elem.next = NULL;
	ft_lstadd_front(NULL, &elem);
}

static void crash_lstadd_front_null_new(void)
{
	t_list	*head;

	head = NULL;
	ft_lstadd_front(&head, NULL);
}

static void crash_lstadd_front_null_new_leaves_unchanged(void)
{
	t_list	*head;
	t_list	n;

	n.content = "node";
	n.next = NULL;
	head = &n;
	ft_lstadd_front(&head, NULL);
	if (head != &n)
		exit(1);
}

void test_lstadd_front(void)
{
	t_list	*head;
	t_list	n1;
	t_list	n2;
	t_list	n3;

	printf("\n--- Testing ft_lstadd_front ---\n");

	head = NULL;
	n1.content = "node 1";
	n1.next = NULL;
	n2.content = "node 2";
	n2.next = NULL;
	n3.content = "node 3";
	n3.next = NULL;

	/* 1. Add to empty list (*lst == NULL) */
	ft_lstadd_front(&head, &n1);
	check_ptr(head, &n1, "ft_lstadd_front sets *lst to new on empty list");
	if (head != NULL)
		check_ptr(head->next, NULL, "head->next is NULL after adding first element");

	/* 2. Add second node to front (prepending) */
	ft_lstadd_front(&head, &n2);
	check_ptr(head, &n2, "ft_lstadd_front updates *lst to second node");
	if (head != NULL)
	{
		check_ptr(head->next, &n1, "head->next points to previous head (n1)");
		if (head->next != NULL)
			check_ptr(head->next->next, NULL, "previous head (n1) remains end of list");
	}

	/* 3. Add third node to front */
	ft_lstadd_front(&head, &n3);
	check_ptr(head, &n3, "ft_lstadd_front updates *lst to third node");
	if (head != NULL)
	{
		check_ptr(head->next, &n2, "node 3 next points to node 2");
		if (head->next != NULL)
		{
			check_ptr(head->next->next, &n1, "node 2 next points to node 1");
			if (head->next->next != NULL)
				check_ptr(head->next->next->next, NULL, "node 1 next is NULL");
		}
	}

	/* 4. Defensive NULL checks */
	check_no_crash(crash_lstadd_front_null_lst, "ft_lstadd_front(NULL, new) handles NULL lst safely (no crash)");
	check_no_crash(crash_lstadd_front_null_new, "ft_lstadd_front(&head, NULL) handles NULL new safely (no crash)");
	check_no_crash(crash_lstadd_front_null_new_leaves_unchanged, "ft_lstadd_front(&head, NULL) leaves list unchanged");
}

