/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 01:29:59 by kamurai           #+#    #+#             */
/*   Updated: 2026/09/29 04:29:26 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

uintmax_t	arg_to_num(char *str)
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
			{
				fprintf(stderr, "Error: argument is too large\n");
				return (-1);
			}
		}
		else
		{
			fprintf(stderr, "Error: argument is not digit\n");
			return (-1);
		}
		i++;
	}
	return (result);
}
