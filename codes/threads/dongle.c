/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/25 13:52:21 by kamurai           #+#    #+#             */
/*   Updated: 2026/11/05 18:32:33 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

/*
** Releases one dongle and records released_at.
*/
static void	release_a_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&(dongle->mutex));
	dongle->in_use = false;
	dongle->released_at = get_time_ms();
	pthread_mutex_unlock(&(dongle->mutex));
}

/*
** Releases both dongles.
*/
void	release_dongles(t_dongle *dongle1, t_dongle *dongle2)
{
	release_a_dongle(dongle1);
	release_a_dongle(dongle2);
}

/*
** Locks the mutexes of both dongles in a fixed order
** (lower address first) to prevent deadlock.
*/
static void	lock_dongles_mutex(t_dongle *dongle1, t_dongle *dongle2)
{
	if (dongle1 > dongle2)
	{
		pthread_mutex_lock(&(dongle2->mutex));
		pthread_mutex_lock(&(dongle1->mutex));
		return ;
	}
	pthread_mutex_lock(&(dongle1->mutex));
	pthread_mutex_lock(&(dongle2->mutex));
}

/*
** Unlocks the mutexes of both dongles.
*/
static void	unlock_dongles_mutex(t_dongle *dongle1, t_dongle *dongle2)
{
	pthread_mutex_unlock(&(dongle1->mutex));
	pthread_mutex_unlock(&(dongle2->mutex));
}

/*
** Puts the request in the queues of both dongles at once,
** then waits until the coder is at the head of both queues
** and both dongles are free. Takes both dongles at the same time.
** While waiting, unlocks both mutexes and checks again every 100 us.
** Return: true if both are taken, false if stop became true
*/
bool	take_dongles(t_coder *coder, t_dongle *first, t_dongle *second)
{
	t_request	request;

	lock_dongles_mutex(first, second);
	request = make_request(coder);
	push_request(request, first, coder->table->config.scheduler);
	push_request(request, second, coder->table->config.scheduler);
	while (!is_my_turn(coder, first) || !is_my_turn(coder, second))
	{
		unlock_dongles_mutex(first, second);
		if (is_stopped(coder->table))
			return (false);
		usleep(100);
		lock_dongles_mutex(first, second);
	}
	first->in_use = true;
	second->in_use = true;
	print_log("take dongle", coder);
	print_log("take dongle", coder);
	remove_request(first);
	remove_request(second);
	return (unlock_dongles_mutex(first, second), true);
}
