/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 04:10:14 by kamurai           #+#    #+#             */
/*   Updated: 2026/10/12 20:40:26 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

// スレッド開始直前の時刻を記録
static void	set_start_time(t_table *table, uintmax_t number_of_coders)
{
	uintmax_t	i;
	
	table->start_time = get_time_ms();
	i = 0;
	while (i < number_of_coders)
		table->corders[i++].last_compile_start = table->start_time;
}

// スレッドの作成
static bool	create_threads(t_table *table, pthread_t *simulate_threads,
	pthread_t *monitor_thread, uintmax_t number_of_coders)
{
	uintmax_t	i;

	i = 0;
	while (i < number_of_coders)
	{
		if (pthread_create(&simulate_threads[i],
			NULL, run_simulation, &table->corders[i]) != 0)
			break;
		i++;
	}
	if (i != number_of_coders
		|| pthread_create(monitor_thread, NULL, monitor_routine, table) != 0)
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

// スレッドを開始して終わるまで待つ
static void	join_threads(pthread_t *simulate_threads,
	pthread_t monitor_thread, uintmax_t number_of_coders)
{
	uintmax_t	i;
	
	i = 0;
	while (i < number_of_coders)
		pthread_join(simulate_threads[i++], NULL);
	pthread_join(monitor_thread, NULL);
}

/* すべてのスレッドを作成/監視/制御する */
bool	run_simulations(t_config config)
{
	t_table		*table;				// corder と dongle が囲む円卓
	pthread_t	*simulate_threads;	// シミュレート用スレッド（number_of_coders個）
	pthread_t	monitor_thread;		// マルチスレッド全体の状態を監視するためのスレッド（1個）

	// テーブルの作成
	if (set_table(&table, config) == false)
		return (false);
	
	// スレッドの作成
	simulate_threads = malloc(sizeof(pthread_t) * config.number_of_coders);
	if (simulate_threads == NULL)
		return (free(simulate_threads), free_table(table), false);

	// スレッド開始直前の時刻を記録
	set_start_time(table, config.number_of_coders);
			
	// スレッドの作成
	if (create_threads(table, simulate_threads,
		&monitor_thread, config.number_of_coders) == false)
		return (free(simulate_threads), free_table(table), false);

	// スレッドを開始して終わるまで待つ
	join_threads(simulate_threads, monitor_thread, config.number_of_coders);
	
	return (free(simulate_threads), free_table(table), true);
}
