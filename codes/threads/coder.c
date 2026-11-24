/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 20:26:40 by kamurai           #+#    #+#             */
/*   Updated: 2026/11/06 01:19:28 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

/*
** Handles the case of a single coder.
** There is only one dongle, so the coder can never compile.
** 1. Takes the dongle and prints "has taken a dongle"
** 2. Waits until stop is true (the monitor detects the burnout)
** 3. Puts the dongle back and returns
*/
static void	*run_simulation_when_one_coder(t_coder *coder)
{
	pthread_mutex_lock(&coder->left->mutex);
	coder->left->in_use = true;
	pthread_mutex_unlock(&coder->left->mutex);
	print_log("take dongle", coder);
	while (!is_stopped(coder->table))
		usleep(500);
	pthread_mutex_lock(&coder->left->mutex);
	coder->left->in_use = false;
	pthread_mutex_unlock(&coder->left->mutex);
	return (NULL);
}

/*
** Handles the case of two or more coders.
** Repeats steps 1 to 4 until stop is true.
** 1. Takes two dongles (returns if stop became true while waiting)
** 2. Compiles
** 3. Releases both dongles
** 4. Debugs and refactors
*/
static void	*run_simulation_when_some_coders(t_coder *coder)
{
	char		*scheduler;

	scheduler = coder->table->config.scheduler;
	if (strcmp(scheduler, "fifo") == 0 && coder->id % 2 == 0)
		sleep_in_500us_steps(coder->table,
			coder->table->config.time_to_compile / 2);
	while (!is_stopped(coder->table))
	{
		if (!take_dongles(coder, coder->right, coder->left))
			break ;
		compiling(coder);
		release_dongles(coder->right, coder->left);
		debugging(coder);
		refactoring(coder);
	}
	return (NULL);
}

/*
** Entry point of a coder thread.
** Chooses the routine based on the number of coders.
*/
void	*run_simulation(void *arg)
{
	t_coder		*coder;
	t_config	*config;

	coder = arg;
	config = &coder->table->config;
	if (config->number_of_coders == 1)
		return (run_simulation_when_one_coder(coder));
	else
		return (run_simulation_when_some_coders(coder));
}
