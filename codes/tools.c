/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 01:29:59 by kamurai           #+#    #+#             */
/*   Updated: 2026/10/02 17:13:31 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdarg.h>

bool	is_scheduler(char *str)
{
	if (strcmp(str, "fifo") == 0 || strcmp(str, "edf") == 0)
		return (true);
	return (false);
}

bool	set_num(char *str, uintmax_t *num)
{
	int	result;
	int	i;

	result = 0;
	i = 0;
	while (str[i])
	{
		if (str[i] >= '0' && str[i] <= '9')
		{
			if (result > UINTMAX_MAX - (str[i] - '0') / 10)
				result += result * 10 + (str[i] - '0');
			else
				return (
					fprintf(stderr, "Error: argument is too large\n"), false);
		}
		else
			return (fprintf(stderr, "Error: argument is not digit\n"), false);
		i++;
	}
	return (*num = result, true);
}

bool	set_scheduler(char *src, char *dst)
{
	size_t	i;

	i = 0;
	while (src[i])
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
	return (true);
}

void	free_all(int count, ...)
{
	va_list	args;
	int		i;

	va_start(args, count);
	i = 0;
	while (i < count)
	{
		free(va_arg(args, void *));
		i++;
	}
	va_end(args);
}
