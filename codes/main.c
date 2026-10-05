/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 01:05:42 by kamurai           #+#    #+#             */
/*   Updated: 2026/10/02 17:17:43 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static t_config	*set_data(char **argv)
{
	t_config	*data;

	data = malloc(sizeof(t_config));
	if (data == NULL)
		return (NULL);

	data->scheduler = malloc(sizeof(char) * (strlen(argv[8]) + 1));
	if (data->scheduler == NULL || is_scheduler(argv[8]) == false)
		return (free_all(2, data, data->scheduler), NULL);

	if (set_num(argv[1], &data->number_of_coders) == false
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

int	main(int argc, char **argv)
{
	t_config	*data;

	if (argc != 9)
		return (fprintf(stderr, "Error: wrong number of argments\n"), 1);

	data = set_data(argv);
	if (data == NULL)
		return (1);

	if (run_simulations(*data) == false)
		return (free_all(2, data, data->scheduler), 1);

	return (free_all(2, data, data->scheduler), 0);
}
