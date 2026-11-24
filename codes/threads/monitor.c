/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/25 13:28:35 by kamurai           #+#    #+#             */
/*   Updated: 2026/11/02 19:37:41 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

/*
** Checks whether time_to_burnout ms or more have passed
** since the coder last started compiling.
** Return: true if so, false otherwise
*/
static bool	is_burned_out(t_coder *coder, t_config *config)
{
	long long	elapsed_ms;

	pthread_mutex_lock(&(coder->mutex));
	elapsed_ms = get_time_ms() - coder->last_compile_start;
	if (elapsed_ms >= config->time_to_burnout)
		return (pthread_mutex_unlock(&(coder->mutex)), true);
	else
		return (pthread_mutex_unlock(&(coder->mutex)), false);
}

/*
** Prints "burned out" (print_log also sets stop to true).
*/
static void	burn_out(t_coder *coder)
{
	print_log("burn out", coder);
}

/*
** Checks whether every coder's compile_count has reached
** number_of_compiles_required.
** Return: true if all have reached it, false otherwise
*/
static bool	is_all_compiled(t_table *table, t_config *config)
{
	int	required;
	int	i;

	required = config->number_of_compiles_required;
	i = 0;
	while (i < config->number_of_coders)
	{
		pthread_mutex_lock(&(table->coders[i].mutex));
		if (table->coders[i].compile_count < required)
			return (pthread_mutex_unlock(&(table->coders[i].mutex)), false);
		pthread_mutex_unlock(&(table->coders[i].mutex));
		i++;
	}
	return (true);
}

/*
** Routine of the monitor thread. Repeats about every 1 ms until stop is true.
** 1. If a coder has burned out, prints "burned out"
**    and sets stop to true
** 2. If all coders have compiled enough times, sets stop to true
**    without printing
*/
void	*monitor_routine(void *arg)
{
	t_table		*table;
	t_config	*config;
	int			i;

	table = arg;
	config = &(table->config);
	while (!is_stopped(table))
	{
		i = 0;
		while (i < config->number_of_coders)
		{
			if (is_burned_out(&(table->coders[i]), config))
				burn_out(&(table->coders[i]));
			i++;
		}
		if (is_all_compiled(table, config))
			stop_simulation(table);
		usleep(1000);
	}
	return (NULL);
}
