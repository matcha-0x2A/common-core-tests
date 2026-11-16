#include "libft_tests.h"

void test_lstlast(void)
{
	t_list	n1;
	t_list	n2;
	t_list	n3;
	t_list	n4;
	t_list	n5;

	printf("\n--- Testing ft_lstlast ---\n");

	/* 1. Empty list (NULL) */
	check_ptr(ft_lstlast(NULL), NULL, "ft_lstlast(NULL) returns NULL");

	/* 2. Single node */
	n1.content = "node 1";
	n1.next = NULL;
	check_ptr(ft_lstlast(&n1), &n1, "ft_lstlast returns single node");

	/* 3. Two nodes */
	n2.content = "node 2";
	n2.next = NULL;
	n1.next = &n2;
	check_ptr(ft_lstlast(&n1), &n2, "ft_lstlast returns node 2 for 2-node list");
	check_ptr(ft_lstlast(&n2), &n2, "ft_lstlast returns node 2 when called on node 2");

	/* 4. Five nodes */
	n3.content = "node 3";
	n3.next = &n4;
	n4.content = "node 4";
	n4.next = &n5;
	n5.content = "node 5";
	n5.next = NULL;
	n2.next = &n3;
	check_ptr(ft_lstlast(&n1), &n5, "ft_lstlast returns node 5 for 5-node list");
	check_ptr(ft_lstlast(&n3), &n5, "ft_lstlast returns node 5 when starting from middle node 3");
	check_ptr(ft_lstlast(&n5), &n5, "ft_lstlast returns node 5 when called on last node");

	/* 5. Verify list structure was not modified */
	check(n1.next == &n2 && n4.next == &n5 && n5.next == NULL,
		"ft_lstlast leaves all next pointers unmodified");
}

