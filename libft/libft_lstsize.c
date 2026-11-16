#include "libft_tests.h"

void test_lstsize(void)
{
	t_list	n1;
	t_list	n2;
	t_list	n3;
	t_list	n4;
	t_list	n5;
	t_list	nodes[100];
	int		i;

	printf("\n--- Testing ft_lstsize ---\n");

	/* 1. Empty list (NULL) */
	check_uint(ft_lstsize(NULL), 0, "ft_lstsize(NULL) returns 0");

	/* 2. Single node */
	n1.content = "1";
	n1.next = NULL;
	check_uint(ft_lstsize(&n1), 1, "ft_lstsize returns 1 for single node");

	/* 3. Two nodes */
	n2.content = "2";
	n2.next = NULL;
	n1.next = &n2;
	check_uint(ft_lstsize(&n1), 2, "ft_lstsize returns 2 for two nodes");
	check_uint(ft_lstsize(&n2), 1, "ft_lstsize returns 1 for sublist from second node");

	/* 4. Three nodes */
	n3.content = "3";
	n3.next = NULL;
	n2.next = &n3;
	check_uint(ft_lstsize(&n1), 3, "ft_lstsize returns 3 for three nodes");

	/* 5. Five nodes */
	n4.content = "4";
	n4.next = &n5;
	n5.content = "5";
	n5.next = NULL;
	n3.next = &n4;
	check_uint(ft_lstsize(&n1), 5, "ft_lstsize returns 5 for five nodes");
	check_uint(ft_lstsize(&n3), 3, "ft_lstsize returns 3 from middle node (node 3)");
	check_uint(ft_lstsize(&n5), 1, "ft_lstsize returns 1 from last node (node 5)");

	/* 6. Verify list was not modified */
	check(n1.next == &n2 && n2.next == &n3 && n3.next == &n4 && n4.next == &n5 && n5.next == NULL,
		"ft_lstsize leaves all next pointers unmodified");

	/* 7. Large list (100 nodes) */
	for (i = 0; i < 100; i++)
	{
		nodes[i].content = NULL;
		nodes[i].next = (i < 99) ? &nodes[i + 1] : NULL;
	}
	check_uint(ft_lstsize(&nodes[0]), 100, "ft_lstsize returns 100 for 100 nodes");
	check_uint(ft_lstsize(&nodes[50]), 50, "ft_lstsize returns 50 starting from node 50");
}

