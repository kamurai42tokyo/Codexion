/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 20:26:40 by kamurai           #+#    #+#             */
/*   Updated: 2026/10/15 02:36:11 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* corder１人のスレッドが、シミュレーション開始からstop=trueになるまで下記の処理を繰り返す。
** 
** 1. 左右のドングルへ申込書（t_request）を出して、scheduler（fifoかedf）の順番が
**    来るまで待ち、２本を取る。
**    cooldown（返されてからdongle_cooldown 秒）が過ぎるまでは取らない。
**    １本取るたびに has taken a dongle のログ出し、time_to_compile秒持つ。
** 2. last_compile_start を今の時刻にして、 is compiling のログを出し、
**    time_to_compiling 秒待つ。
** 3. compile_count を１増やして、２本のドングルを返す。返すときは released_at に
**    今の時刻を入れて、待っとる隣の corder を cond で起こす。
** 4. is debugging のログ出して、time_to_debug秒 待つ。
** 5. is refactoring のログ出して、time_to_refactor秒 待つ。
*/
void    *run_simulation(void *arg)
{
    t_corder    *corder;
    t_config    *config;
    
    corder = arg;
    config = &corder->table->config;

    // corderが１人だけ（=ドングルも１本だけ）の場合
	if (config->number_of_corder)

    // 
    while (1)
    {
        if (left->in_use)
            
    }
    pthread_mutex_lock(&left->mutex);
}

/* シミュレーションが終わるまで全corderを短い間隔で順番に見て回る。
** 「燃え尽き」もしくは「全員完了」の判定を実施する。
** 
** 1. 「今の時刻（last_compile_start）」が time_to_burnout 以上の corder が１人でも
**    おったら、burned out のログを出して stop を ture にする。
** 2. 全員の compile_count が number_of_compiles_required 以上になっとったら、
**    ログは出さずに step を ture にする。
** 3. stop を true にしたら、全ドングルの cond で寝とる corder スレッドを起こして、
**    終わらせる。
*/ 
void    *monitor_routine(void *arg)
{
	
}
