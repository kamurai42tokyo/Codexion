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

/* ①コンパイル開始
** ・coder->mutex で鍵をかけて、last_compile_start を今の時刻にする（monitor も読む値やから鍵がいる）
** ・is compiling のログを出す
** ・time_to_compile ミリ秒待つ（README どおり 500µs ずつ刻んで、stop も確認する）
** ②コンパイル終了
** ・coder->mutex で鍵をかけて、compile_count を1増やす */
void    compiling(t_coder *coder)
{
    t_table *table;

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

/* デバッグ: is debugging のログを出して、time_to_debug ミリ秒待つ */
void    debugging(t_coder *coder)
{
    t_table *table;

    table = coder->table;
    
    print_log("debugging", coder);
    
    sleep_in_500us_steps(table, table->config.time_to_debug);
}

/* リファクタ: is refactoring のログを出して、time_to_refactor ミリ秒待つ */
void    refactoring(t_coder *coder)
{
    t_table *table;

    table = coder->table;
    
    print_log("refactoring", coder);
    
    sleep_in_500us_steps(table, table->config.time_to_refactor);
}
