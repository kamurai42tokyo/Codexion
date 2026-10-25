/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/25 13:37:49 by kamurai           #+#    #+#             */
/*   Updated: 2026/10/25 20:04:33 by kamurai          ###   ########.fr       */
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

bool	is_my_turn(t_corder *corder, t_dongle *dongle)
{
	
}
