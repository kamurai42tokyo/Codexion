/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 04:10:14 by kamurai           #+#    #+#             */
/*   Updated: 2026/10/01 07:28:33 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	*run_simulation(t_corder corder)
{
	
}

t_corder	*set_corders(t_data data)
{
	t_corder	*corders;
	int			i;

	corders = malloc(sizeof(t_corder) * data.number_of_coders);
	if (corders == NULL)
		return (NULL);

	i = 0;
	while (i < data.number_of_coders)
	{
		
		i++;
	}
	
}

bool	run_simulations(t_data data)
{
	pthread_t	*threads;
	t_corder	*corders;
	uintmax_t	i;

	threads = malloc(sizeof(pthread_t) * data.number_of_coders);
	corders = set_corders(data);
	if (threads == NULL || corders == NULL)
		return (false);


	i = 0;
	while (i < data.number_of_coders)
	{
		pthread_create(&threads[i], NULL, run_simulation, &coders[i]);
		
	}
	pthread_join(threads, NULL);
}
