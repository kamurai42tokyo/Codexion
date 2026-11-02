/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 01:05:42 by kamurai           #+#    #+#             */
/*   Updated: 2026/11/02 14:44:53 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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
