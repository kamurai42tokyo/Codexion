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

// dongle が cooldown 中かを判定する。
bool	is_cooldown(t_coder *coder, t_dongle *dongle)
{
	t_config	*config;
	long long	elapsed;
	
	elapsed = get_time_ms() - dongle->released_at;
	config = &(coder->table->config);
	if (elapsed < (long long)config->dongle_cooldown)
		return (true);
	else
		return (false);
}

/* この dongle がこの coder のものかを判定する。
** [判定１] dongle->request_count が 0 件なら false
** [判定２] dongle->in_use が true なら false
** [判定３] dongle が cooldown 中なら false
** [判定４] fifo/edf に応じた方法で、この coder が次に dongle を取れるか判定
** 　-　fifo の場合: request_queue[0] が coder のものなら true
** 　-　edf  の場合: request_queue のうち deadline が早い方が coder のものなら true
*/
bool	is_my_turn(t_coder *coder, t_dongle *dongle)
{
	t_config	*config;
	bool		in_cooldown;
	
	config = &(coder->table->config);
	
	// 1
	in_cooldown = is_cooldown(coder, dongle);
	if (!dongle->request_count || dongle->in_use || in_cooldown)
		return (false);

	// 2
	if (strcmp(config->scheduler, "fifo") == 0 && \
			coder->id == dongle->request_queue[0].id)
		return (true);
	else if (strcmp(config->scheduler, "edf") == 0 && \
			coder->id == get_earliest_deadline_id(dongle))
		return (true);
	else
		return (false);
}
