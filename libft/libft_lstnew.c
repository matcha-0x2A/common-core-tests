#include "libft_tests.h"

void test_lstnew(void)
{
	char	str[] = "Hello 42";
	int		val;
	t_list	*node;
	t_list	*n1;
	t_list	*n2;

	printf("\n--- Testing ft_lstnew ---\n");

	/* 1. Basic node creation with string content */
	node = ft_lstnew(str);
	check(node != NULL, "ft_lstnew returns non-NULL for valid string content");
	if (node != NULL)
	{
		check(node->content == str, "ft_lstnew stores exact content pointer");
		check(strcmp((char *)node->content, "Hello 42") == 0, "ft_lstnew content string matches");
		check(node->next == NULL, "ft_lstnew initializes next to NULL");
		free(node);
	}

	/* 2. Node creation with integer pointer content */
	val = 1337;
	node = ft_lstnew(&val);
	check(node != NULL, "ft_lstnew returns non-NULL for int pointer");
	if (node != NULL)
	{
		check(node->content == &val, "ft_lstnew stores address of int correctly");
		check(*(int *)node->content == 1337, "ft_lstnew value of int matches 1337");
		check(node->next == NULL, "ft_lstnew initializes next to NULL for int node");
		free(node);
	}

	/* 3. Node creation with NULL content */
	node = ft_lstnew(NULL);
	check(node != NULL, "ft_lstnew returns non-NULL when content is NULL");
	if (node != NULL)
	{
		check(node->content == NULL, "ft_lstnew sets content to NULL");
		check(node->next == NULL, "ft_lstnew initializes next to NULL with NULL content");
		free(node);
	}

	/* 4. Recycled heap memory (catches missing `new->next = NULL;`) */
	void *dirty = malloc(sizeof(t_list));
	if (dirty != NULL)
	{
		memset(dirty, 0x5A, sizeof(t_list));
		free(dirty);
		node = ft_lstnew("recycled");
		check(node != NULL, "ft_lstnew returns non-NULL on recycled chunk");
		if (node != NULL)
		{
			check(node->next == NULL, "ft_lstnew explicitly sets next to NULL on recycled chunk");
			free(node);
		}
	}

	/* 5. Multiple independent nodes allocated */
	n1 = ft_lstnew("first");
	n2 = ft_lstnew("second");
	check(n1 != NULL && n2 != NULL && n1 != n2, "ft_lstnew allocates distinct memory addresses");
	if (n1 != NULL)
	{
		check(n1->next == NULL, "n1 next is NULL");
		free(n1);
	}
	if (n2 != NULL)
	{
		check(n2->next == NULL, "n2 next is NULL");
		free(n2);
	}
}

