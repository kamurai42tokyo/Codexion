/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/25 13:37:49 by kamurai           #+#    #+#             */
/*   Updated: 2026/11/02 15:03:52 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

// stop が true か false かを判定して返す。
bool	is_stopped(t_table *table)
{
	pthread_mutex_lock(&table->stop_mutex);
	if (table->stop == false)
		return (pthread_mutex_unlock(&table->stop_mutex), false);
	return (pthread_mutex_unlock(&table->stop_mutex), true);
}

void	stop_simulation(t_table *table)
{
    pthread_mutex_lock(&(table->stop_mutex));
    table->stop = true;
    pthread_mutex_unlock(&(table->stop_mutex));
}
