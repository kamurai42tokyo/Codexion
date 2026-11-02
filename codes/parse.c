/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 14:44:55 by kamurai           #+#    #+#             */
/*   Updated: 2026/11/02 14:46:32 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_config	*set_data(char **argv)
{
	t_config	*data;

	data = malloc(sizeof(t_config));
	if (data == NULL)
		return (NULL);

	data->scheduler = malloc(sizeof(char) * (strlen(argv[8]) + 1));
	if (data->scheduler == NULL || is_scheduler(argv[8]) == false)
		return (free_all(2, data, data->scheduler), NULL);

	if (set_num(argv[1], &data->number_of_coders) == false
		|| data->number_of_coders == 0
		|| set_num(argv[2], &data->time_to_burnout) == false
		|| set_num(argv[3], &data->time_to_compile) == false
		|| set_num(argv[4], &data->time_to_debug) == false
		|| set_num(argv[5], &data->time_to_refactor) == false
		|| set_num(argv[6], &data->number_of_compiles_required) == false
		|| set_num(argv[7], &data->dongle_cooldown) == false
		|| set_scheduler(argv[8], data->scheduler) == false)
		return (free_all(2, data, data->scheduler), NULL);

	return (data);
}

bool	is_scheduler(char *str)
{
	if (strcmp(str, "fifo") == 0 || strcmp(str, "edf") == 0)
		return (true);
	return (false);
}

bool	set_num(char *str, uintmax_t *num)
{
	uintmax_t	result;
	uintmax_t	i;

	if (str[0] == '\0')
		return (fprintf(stderr, "Error: argument is empty\n"), false);
	result = 0;
	i = 0;
	while (str[i])
	{
		if (str[i] >= '0' && str[i] <= '9')
		{
			if (result > (UINTMAX_MAX - (str[i] - '0')) / 10)
				return (
					fprintf(stderr, "Error: argument is too large\n"), false);
			else
				result = result * 10 + (str[i] - '0');
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
