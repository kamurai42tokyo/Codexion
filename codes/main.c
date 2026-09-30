/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 01:05:42 by kamurai           #+#    #+#             */
/*   Updated: 2026/09/29 04:34:51 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

bool	set_data(char **argv, t_data *data)
{
	data.number_of_coders = arg_to_num(argv[1]);
	data.time_to_burnout = arg_to_num(argv[2]);
	data.time_to_compile = arg_to_num(argv[3]);
	data.time_to_debug = arg_to_num(argv[4]);
	data.time_to_refactor = arg_to_num(argv[5]);
	data.number_of_compiles_required = arg_to_num(argv[6]);
	data.dongle_cooldown = arg_to_num(argv[8]);
	data.scheduler = argv[9];
	return True
}

int	main(int argc, char **argv)
{
	t_data	data;

	if (argc != 9)
	{
		fprintf(stderr, "Error: wrong number of argments\n");
		return (1);
	}

	set_data(argv, data);
}