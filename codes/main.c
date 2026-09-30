/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 01:05:42 by kamurai           #+#    #+#             */
/*   Updated: 2026/10/01 06:55:11 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static t_data	*set_data(char **argv)
{
	t_data	*data;

	data = malloc(sizeof(t_data));
	if (data == NULL)
		return (NULL);

	data->scheduler = malloc(sizeof(char) * strlen(argv[9]));
	if (data->scheduler == NULL || is_scheduler(argv[9]) == NULL)
		return (free(data), NULL);

	set_num(argv[1], &data->number_of_coders);
	set_num(argv[2], &data->time_to_burnout);
	set_num(argv[3], &data->time_to_compile);
	set_num(argv[4], &data->time_to_debug);
	set_num(argv[5], &data->time_to_refactor);
	set_num(argv[6], &data->number_of_compiles_required);
	set_num(argv[7], &data->dongle_cooldown);
	set_scheduler(argv[8], &data->scheduler);
	data->scheduler = argv[9];

	return (data);
}

int	main(int argc, char **argv)
{
	t_data	*data;

	if (argc != 9)
		return (fprintf(stderr, "Error: wrong number of argments\n"), 1);

	data = set_data(argv);

	if (run_simulations(data) == false)
		return (free_all(data), 1);

	return (0);
}
