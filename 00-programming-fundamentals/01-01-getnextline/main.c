/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mchambon <mchambon@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/18 15:51:45 by mchambon          #+#    #+#             */
/*   Updated: 2026/11/18 15:51:47 by mchambon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define COLOR_GREEN "\033[32m"
#define COLOR_RED   "\033[31m"
#define COLOR_RESET "\033[0m"

int	main(int argc, char **argv)
{
	int		fd;
	char	*line;

	fd = 0;
	if (argc > 1)
		fd = open(argv[1], O_RDONLY);
	line = get_next_line(fd);
	while (line)
	{
		printf("%s[%s%s%s]%s",
			COLOR_GREEN, COLOR_RESET, line, COLOR_RED, COLOR_RESET);
		free(line);
		line = get_next_line(fd);
	}
	if (fd > 0)
		close(fd);
	return (0);
}
