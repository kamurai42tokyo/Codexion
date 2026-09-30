/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 01:29:59 by kamurai           #+#    #+#             */
/*   Updated: 2026/10/01 03:52:11 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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
