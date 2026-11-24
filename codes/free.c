/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 17:31:27 by kamurai           #+#    #+#             */
/*   Updated: 2026/10/13 00:11:59 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** Frees all count pointers passed as variadic arguments.
*/
void	free_all(int count, ...)
{
	va_list	args;
	int		i;

	va_start(args, count);
	i = 0;
	while (i < count)
	{
		free(va_arg(args, void *));
		i++;
	}
	va_end(args);
}

/*
** Destroys the mutex of the first count dongles.
*/
void	destroy_dongle_mutexes(t_dongle *dongles, int count)
{
	while (count > 0)
		pthread_mutex_destroy(&dongles[--count].mutex);
}

/*
** Destroys the mutex of the first count coders.
*/
void	destroy_coder_mutexes(t_coder *coders, int count)
{
	while (count > 0)
		pthread_mutex_destroy(&coders[--count].mutex);
}

/*
** Destroys stop_mutex and log_mutex of the table.
*/
void	destroy_table_mutexes(t_table *table)
{
	pthread_mutex_destroy(&table->stop_mutex);
	pthread_mutex_destroy(&table->log_mutex);
}

/*
** Destroys all mutexes held by the table,
** then frees dongles, coders and the table.
*/
void	free_table(t_table *table)
{
	int	count;

	count = table->config.number_of_coders;
	destroy_dongle_mutexes(table->dongles, count);
	destroy_coder_mutexes(table->coders, count);
	destroy_table_mutexes(table);
	free_all(3, table->dongles, table->coders, table);
}
