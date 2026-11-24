/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 14:44:55 by kamurai           #+#    #+#             */
/*   Updated: 2026/11/15 00:51:05 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** Checks whether str is "fifo" or "edf".
** Return: true if it is either, false otherwise
*/
static bool	is_scheduler(char *str)
{
	if (strcmp(str, "fifo") == 0 || strcmp(str, "edf") == 0)
		return (true);
	return (false);
}

/*
** Converts str to a non-negative integer (up to INT_MAX).
** Stores it in *num_int if num_int is not NULL, otherwise in *num_ll.
** Return: true on success, false if empty, non-digit or overflow
*/
static bool	set_num(char *str, long long *num_ll, int *num_int)
{
	long long	result;
	int			i;

	if (str[0] == '\0')
		return (fprintf(stderr, "Error: argument is empty\n"), false);
	result = 0;
	i = 0;
	while (str[i])
	{
		if (str[i] >= '0' && str[i] <= '9')
		{
			if (result > (INT_MAX - (str[i] - '0')) / 10)
				return (fprintf(stderr, "Error: argument is too large\n"),
					false);
			else
				result = result * 10 + (str[i] - '0');
		}
		else
			return (fprintf(stderr, "Error: argument is not digit\n"), false);
		i++;
	}
	if (num_int != NULL)
		return (*num_int = (int)result, true);
	return (*num_ll = result, true);
}

static bool	is_num_of_corders_positive(int num)
{
	if (num == 0)
	{
		fprintf(stderr, "Error: number_of_coders must be > 0\n");
		return (false);
	}
	return (true);
}

/*
** Copies the string src into dst.
*/
static void	set_scheduler(char *src, char *dst)
{
	size_t	i;

	i = 0;
	while (src[i])
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
}

/*
** Creates a t_config from argv and returns it.
** Returns NULL if any argument is invalid.
*/
t_config	*set_data(char **argv)
{
	t_config	*data;

	if (is_scheduler(argv[8]) == false)
		return (fprintf(stderr, "Error: scheduler must be fifo or edf\n"),
			NULL);
	data = malloc(sizeof(t_config));
	if (data == NULL)
		return (NULL);
	data->scheduler = malloc(sizeof(char) * (strlen(argv[8]) + 1));
	if (data->scheduler == NULL)
		return (free_all(2, data, data->scheduler), NULL);
	if (set_num(argv[1], NULL, &data->number_of_coders) == false
		|| is_num_of_corders_positive(data->number_of_coders) == false
		|| set_num(argv[2], &data->time_to_burnout, NULL) == false
		|| set_num(argv[3], &data->time_to_compile, NULL) == false
		|| set_num(argv[4], &data->time_to_debug, NULL) == false
		|| set_num(argv[5], &data->time_to_refactor, NULL) == false
		|| set_num(argv[6], NULL, &data->number_of_compiles_required) == false
		|| set_num(argv[7], &data->dongle_cooldown, NULL) == false)
		return (free_all(2, data, data->scheduler), NULL);
	set_scheduler(argv[8], data->scheduler);
	return (data);
}
