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

static bool is_burned_out(t_coder *coder, t_config *config)
{
    long long   elapsed_ms;

    pthread_mutex_lock(&(coder->mutex));
    elapsed_ms = get_time_ms() - coder->last_compile_start;
    if (elapsed_ms >= (long long)config->time_to_burnout)
        return (pthread_mutex_unlock(&(coder->mutex)), true);
    else
        return (pthread_mutex_unlock(&(coder->mutex)), false);
}

static void burn_out(t_coder *coder)
{
    print_log("burn out", coder);
}

// 全員の compile_count が number_of_compiles_required 以上かを判定する。
static bool    is_all_compiled(t_table *table, t_config *config)
{
    uintmax_t   required;
    uintmax_t   i;

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

/* table->stop が true のとき、全 dongle の cond で寝ている
** 全 coder スレッドを起こして、スレッドを終了させる */
static void wake_all_coders(t_table *table)
{
    uintmax_t   i;

    i = 0;
    while (i < table->config.number_of_coders)
    {
        pthread_mutex_lock(&(table->dongles[i].mutex));
        pthread_cond_broadcast(&(table->dongles[i].cond));
        pthread_mutex_unlock(&(table->dongles[i].mutex));
        i++;
    }
}

/* シミュレーションが終わるまで全coderを短い間隔で順番に見て回る。
** 「燃え尽き」もしくは「全員完了」の判定を実施する。
** 
** 1. 「今の時刻 - last_compile_start」が time_to_burnout 以上の coder が１人でも
**    おったら、burned out のログを出して stop を true にする。
** 2. 全員の compile_count が number_of_compiles_required 以上になっとったら、
**    ログは出さずに stop を ture にする。
** 3. stop を true にしたら、全ドングルの cond で寝とる coder スレッドを起こして、
**    終わらせる。*/ 
void    *monitor_routine(void *arg)
{
    t_table     *table;
    t_config    *config;
    uintmax_t   i;

    table = arg;
    config = &(table->config);

    while (!is_stopped(table))
    {
        i = 0;
        while (i < config->number_of_coders)
        {
            if (is_burned_out(&(table->coders[i]), &(table->config)))
                burn_out(&(table->coders[i]));
            if (is_all_compiled(table, &(table->config)))
                stop_simulation(table);
            i++;
        }
        usleep(1000);
    }
    wake_all_coders(table);
    return (NULL);
}
