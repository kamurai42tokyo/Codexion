/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 01:29:59 by kamurai           #+#    #+#             */
/*   Updated: 2026/11/05 16:03:48 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

/*
** Returns the time elapsed since 1970-01-01 00:00:00 UTC in ms.
*/
long long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((long long)tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

/*
** Waits ms in total, in steps of 500 us.
** Returns early if stop becomes true.
*/
void	sleep_in_500us_steps(t_table *table, long long ms)
{
	long long	start;

	start = get_time_ms();
	while (get_time_ms() - start < ms)
	{
		if (is_stopped(table))
			return ;
		usleep(500);
	}
}
