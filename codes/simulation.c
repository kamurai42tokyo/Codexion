/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 04:10:14 by kamurai           #+#    #+#             */
/*   Updated: 2026/10/03 07:50:38 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_corder	*set_corders(t_config data)
{
	t_corder	*corders;
	uintmax_t	i;

	corders = malloc(sizeof(t_corder) * data.number_of_coders);
	if (corders == NULL)
		return (NULL);

	i = 0;
	while (i < data.number_of_coders)
	{
		corders[i].id = i + 1;
		corders[i].last_compile_start = 0;
		corders[i].compile_count = 0;
		i++;
	}
	return (corders);
}

static void	*run_simulation(void *arg)
{
	
}

/* シミュレーションが終わるまで全corderを短い間隔で順番に見て回る。
** 「燃え尽き」もしくは「全員完了」の判定を実施する。*/ 
static void *monitor_routine(void *arg)
{
	
}

bool	run_simulations(t_config data)
{
	pthread_t	*threads;
	pthread_t	monitor;
	t_corder	*corders;
	t_table		*table;
	t_dongle	*dongle;
	uintmax_t	i;

	threads = malloc(sizeof(pthread_t) * data.number_of_coders);
	corders = set_corders(data);
	// table = set_table
	if (threads == NULL || corders == NULL)
		return (free_all(2, threads, corders), false);

	i = 0;
	while (i < data.number_of_coders)
	{
		if (pthread_create(&threads[i], NULL, run_simulation, &corders[i]) != 0)
			break;
		i++;
	}
	if (i != data.number_of_coders
		|| pthread_create(&monitor, NULL, monitor_routine, corders) != 0)
	{
		while (i > 0)
			pthread_join(threads[--i], NULL);
		return (free_all(2, threads, corders), false);
	}

	while (i > 0)
		pthread_join(threads[--i], NULL);
	pthread_join(monitor, NULL);
	return (free_all(2, threads, corders), true);
}
