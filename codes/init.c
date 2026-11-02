/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/10 23:11:31 by kamurai           #+#    #+#             */
/*   Updated: 2026/10/25 20:04:43 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void    set_request_queue(t_request *request_queue)
{
    int i;

    i = 0;
    while (i < 2)
    {
        request_queue[i].id = 0;
        request_queue[i].deadline = 0;
        request_queue[i].arrival_order = 0;
        i++;
    }
}

bool	set_dongles(t_table *table)
{
    t_dongle    *dongles;
    t_config    config;
    uintmax_t	i;

    config = table->config;
    dongles = malloc(sizeof(t_dongle) * config.number_of_coders);
    if (dongles == NULL)
        return (false);
    i = 0;
    while (i < config.number_of_coders)
    {
        dongles[i].in_use = false;
        dongles[i].released_at = 0;
        set_request_queue(dongles[i].request_queue);
        dongles[i].request_count = 0;
        dongles[i].next_arrival_order = 1;
        if (pthread_mutex_init(&dongles[i].mutex, NULL) != 0)
            return (destroy_dongle_mutexes(dongles, i), free(dongles), false);
        if (pthread_cond_init(&dongles[i].cond, NULL) != 0)
            return (pthread_mutex_destroy(&dongles[i].mutex),
                destroy_dongle_mutexes(dongles, i), free(dongles), false);
        i++;
    }
    table->dongles = dongles;
    return (true);
}

bool	set_coders(t_table *table)
{
	t_coder	*coders;
    uintmax_t   number_of_coders;
    uintmax_t   i;

    number_of_coders = (table->config).number_of_coders;
	coders = malloc(sizeof(t_coder) * number_of_coders);
	if (coders == NULL)
		return (false);
	i = 0;
	while (i < number_of_coders)
	{
		coders[i].id = i + 1;
		coders[i].last_compile_start = 0;
		coders[i].compile_count = 0;
        coders[i].table = table;
        coders[i].right = &(table->dongles[i]);
        coders[i].left = &(table->dongles[(i + 1) % number_of_coders]);
        if (pthread_mutex_init(&coders[i].mutex, NULL) != 0)
            return (destroy_coder_mutexes(coders, i), free(coders), false);
		i++;
	}
    table->coders = coders;
	return (true);
}

bool    set_table(t_table **table, t_config config)
{
    t_table     *new_table;
    
    new_table = malloc(sizeof(t_table));
    if (new_table == NULL)
        return (false);
    new_table->config = config;
    new_table->stop = false;
    if (pthread_mutex_init(&new_table->stop_mutex, NULL) != 0)
        return (free(new_table), false);
    if (pthread_mutex_init(&new_table->log_mutex, NULL) != 0)
        return (pthread_mutex_destroy(&new_table->stop_mutex),
            free(new_table), false);
    if (set_dongles(new_table) == false)
        return (destroy_table_mutexes(new_table), free(new_table), false);
    if (set_coders(new_table) == false)
        return (
            destroy_dongle_mutexes(
                new_table->dongles, config.number_of_coders),
            free(new_table->dongles), destroy_table_mutexes(new_table),
            free(new_table), false);
    *table = new_table;
    return (true);
}
