/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 20:26:40 by kamurai           #+#    #+#             */
/*   Updated: 2026/10/17 12:15:17 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

// stop が true か false かを判定して返す。
static bool	is_stopped(t_table *table)
{
	pthread_mutex_lock(&table->stop_mutex);
	if (table->stop == false)
		return (pthread_mutex_unlock(&table->stop_mutex), false);
	return (pthread_mutex_unlock(&table->stop_mutex), true);
}

/* corder が１人（＝ dongle が１つ）の場合の処理。下記を実施する。
** ① corder->left が指すドングルの in_use を true にする（mutex で守る）。
**    ※円卓にあるのはこれ一本のみ。mutex は握りっぱなしにしない。
** ② “<時刻> <id> has taken a dongle” というログを出力
** ③ stop=true になるまで、「stop を見る → まだ false なら少し寝る → また見る」
**    を繰り返す。※stop=true になったら抜ける。
** ④ ドングルの in_use を false に戻す（mutex で守る）。
** ⑤ NULL を返して終わる。 */
static void	*run_simulation_when_one_corder(t_corder *corder)
{
	pthread_mutex_lock(&corder->left->mutex);
	corder->left->in_use = true;
	pthread_mutex_unlock(&corder->left->mutex);
	pthread_mutex_lock(&corder->table->log_mutex);
	fprintf(stdout, "%lld ", get_time_ms() - corder->table->start_time);
	fprintf(stdout, "%ju ", corder->id);
	fprintf(stdout, "has taken a dongle\n");
	pthread_mutex_unlock(&corder->table->log_mutex);
	while (!is_stopped(corder->table))
		usleep(500);
	pthread_mutex_lock(&corder->left->mutex);
	corder->left->in_use = false;
	pthread_mutex_unlock(&corder->left->mutex);
	return (NULL);
}

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
** 5. is refactoring のログ出して、time_to_refactor秒 待つ。*/
void	*run_simulation(void *arg)
{
	t_corder    *corder;
    t_config	*config;

	corder = arg;
	config = &corder->table->config;

	// corderが１人だけ（=ドングルも１本だけ）の場合
	if (config->number_of_coders == 1)
		return (run_simulation_when_one_corder(corder));
	// 
	while (1)
	{
		
		 
	}
}

/* シミュレーションが終わるまで全corderを短い間隔で順番に見て回る。
** 「燃え尽き」もしくは「全員完了」の判定を実施する。
** 
** 1. 「今の時刻（last_compile_start）」が time_to_burnout 以上の corder が１人でも
**    おったら、burned out のログを出して stop を ture にする。
** 2. 全員の compile_count が number_of_compiles_required 以上になっとったら、
**    ログは出さずに step を ture にする。
** 3. stop を true にしたら、全ドングルの cond で寝とる corder スレッドを起こして、
**    終わらせる。*/ 
void    *monitor_routine(void *arg)
{
	
}
