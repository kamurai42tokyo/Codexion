/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 04:10:14 by kamurai           #+#    #+#             */
/*   Updated: 2026/11/19 21:57:02 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** Records the start time and sets every coder's
** last_compile_start to it.
*/
static void	set_start_time(t_table *table, int number_of_coders)
{
	int	i;

	table->start_time = get_time_ms();
	i = 0;
	while (i < number_of_coders)
		table->coders[i++].last_compile_start = table->start_time;
}

/*
** Creates the coder threads and the monitor thread.
** On failure, sets stop to true and joins the threads
** already created.
** Return: true if all threads are created, false otherwise
*/
static bool	create_threads(t_table *table, pthread_t *simulate_threads,
		pthread_t *monitor_thread, int number_of_coders)
{
	int	i;

	i = 0;
	while (i < number_of_coders)
	{
		if (pthread_create(&simulate_threads[i], NULL, run_simulation,
				&table->coders[i]) != 0)
			break ;
		i++;
	}
	if (i != number_of_coders || pthread_create(monitor_thread, NULL,
			monitor_routine, table) != 0)
	{
		pthread_mutex_lock(&table->stop_mutex);
		table->stop = true;
		pthread_mutex_unlock(&table->stop_mutex);
		while (i > 0)
			pthread_join(simulate_threads[--i], NULL);
		return (false);
	}
	return (true);
}

/*
** Waits for all coder threads and the monitor thread to end.
*/
static void	join_threads(pthread_t *simulate_threads, pthread_t monitor_thread,
		int number_of_coders)
{
	int	i;

	i = 0;
	while (i < number_of_coders)
		pthread_join(simulate_threads[i++], NULL);
	pthread_join(monitor_thread, NULL);
}

/*
** Prepares the table and threads, runs the simulation,
** and frees everything when it ends.
** Return: true on success, false if preparation fails
*/
bool	run_simulations(t_config config)
{
	t_table		*table;
	pthread_t	*simulate_threads;	// corder[0], corder[1], ...
	pthread_t	monitor_thread;

	if (config.number_of_compiles_required == 0)
		return (true);
	if (set_table(&table, config) == false)
		return (false);
	simulate_threads = malloc(sizeof(pthread_t) * config.number_of_coders);
	if (simulate_threads == NULL)
		return (free_table(table), false);
	set_start_time(table, config.number_of_coders);
	if (create_threads(table, simulate_threads, &monitor_thread,
			config.number_of_coders) == false)
		return (free(simulate_threads), free_table(table), false);
	join_threads(simulate_threads, monitor_thread, config.number_of_coders);
	return (free(simulate_threads), free_table(table), true);
}
