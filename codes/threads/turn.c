/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   turn.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 14:58:52 by kamurai           #+#    #+#             */
/*   Updated: 2026/11/02 14:59:54 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

/*
** Checks whether the dongle is in cooldown
** (less than dongle_cooldown ms since it was released).
** Return: true if in cooldown, false otherwise
*/
static bool	is_cooldown(t_coder *coder, t_dongle *dongle)
{
	t_config	*config;
	long long	elapsed;

	elapsed = get_time_ms() - dongle->released_at;
	config = &(coder->table->config);
	if (elapsed < config->dongle_cooldown)
		return (true);
	else
		return (false);
}

/*
** Checks whether the coder can take this dongle now.
** 1. false if there is no request, it is in use, or in cooldown
** 2. true if request_queue[0] is this coder
**    (the head is next for both fifo and edf)
** Return: true if it can be taken, false otherwise
*/
bool	is_my_turn(t_coder *coder, t_dongle *dongle)
{
	if (!dongle->request_count || dongle->in_use || is_cooldown(coder, dongle))
		return (false);
	return (coder->id == dongle->request_queue[0].id);
}
