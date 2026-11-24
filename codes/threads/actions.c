/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/31 07:28:29 by kamurai           #+#    #+#             */
/*   Updated: 2026/11/02 14:54:02 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

/*
** Compiles once.
** 1. Sets last_compile_start to now (guarded by coder->mutex)
** 2. Prints "is compiling" and waits time_to_compile ms
** 3. Increments compile_count (guarded by coder->mutex)
*/
void	compiling(t_coder *coder)
{
	t_table	*table;

	table = coder->table;
	pthread_mutex_lock(&(coder->mutex));
	coder->last_compile_start = get_time_ms();
	pthread_mutex_unlock(&(coder->mutex));
	print_log("compiling", coder);
	sleep_in_500us_steps(table, table->config.time_to_compile);
	pthread_mutex_lock(&(coder->mutex));
	coder->compile_count++;
	pthread_mutex_unlock(&(coder->mutex));
}

/*
** Prints "is debugging" and waits time_to_debug ms.
*/
void	debugging(t_coder *coder)
{
	t_table	*table;

	table = coder->table;
	print_log("debugging", coder);
	sleep_in_500us_steps(table, table->config.time_to_debug);
}

/*
** Prints "is refactoring" and waits time_to_refactor ms.
*/
void	refactoring(t_coder *coder)
{
	t_table	*table;

	table = coder->table;
	print_log("refactoring", coder);
	sleep_in_500us_steps(table, table->config.time_to_refactor);
}
