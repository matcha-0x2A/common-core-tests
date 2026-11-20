#include "libft_tests.h"

void test_lst_lifecycle(void)
{
	t_list	*head;
	t_list	*n1;
	t_list	*n2;
	t_list	*n3;
	t_list	*curr;
	t_list	*tmp;
	int		count;

	printf("\n--- Testing linked list integration lifecycle ---\n");

	head = NULL;
	n1 = ft_lstnew("first");
	n2 = ft_lstnew("second");
	n3 = ft_lstnew("third");

	if (!n1 || !n2 || !n3)
	{
		check(0, "ft_lstnew allocated nodes for lifecycle");
		if (n1)
			free(n1);
		if (n2)
			free(n2);
		if (n3)
			free(n3);
		return ;
	}
	/* Ensure next pointers are NULL even if ft_lstnew forgot to initialize them */
	n1->next = NULL;
	n2->next = NULL;
	n3->next = NULL;

	/* 1. Add n2 first to empty list */
	ft_lstadd_back(&head, n2);
	/* 2. Add n1 in front: n1 -> n2 */
	ft_lstadd_front(&head, n1);
	/* 3. Add n3 in back: n1 -> n2 -> n3 */
	ft_lstadd_back(&head, n3);

	check(head != NULL, "Lifecycle: head is not NULL");
	check(ft_lstsize(head) == 3, "Lifecycle: list size is 3");
	check(ft_lstlast(head) == n3, "Lifecycle: last node is n3");

	/* Clean up safely without infinite loops */
	count = 0;
	curr = head;
	while (curr != NULL && count < 10)
	{
		tmp = curr->next;
		free(curr);
		curr = tmp;
		count++;
	}
}

