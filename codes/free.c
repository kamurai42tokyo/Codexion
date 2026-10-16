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

void    destroy_dongle_mutexes(t_dongle *dongles, uintmax_t count)
{
    while (count > 0)
    {
        count--;
        pthread_mutex_destroy(&dongles[count].mutex);
        pthread_cond_destroy(&dongles[count].cond);
    }
}

void    destroy_corder_mutexes(t_corder *corders, uintmax_t count)
{
    while (count > 0)
        pthread_mutex_destroy(&corders[--count].mutex);
}

void	destroy_table_mutexes(t_table *table)
{
	pthread_mutex_destroy(&table->stop_mutex);
	pthread_mutex_destroy(&table->log_mutex);
}

void    free_table(t_table *table)
{
    uintmax_t   count;

    count = table->config.number_of_coders;
    destroy_dongle_mutexes(table->dongles, count);
    destroy_corder_mutexes(table->corders, count);
    destroy_table_mutexes(table);
    free_all(3, table->dongles, table->corders, table);
}
