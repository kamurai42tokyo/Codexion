/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulates.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 20:26:40 by kamurai           #+#    #+#             */
/*   Updated: 2026/10/25 21:33:37 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

/* corder が１人（＝ dongle が１つ）の場合の処理。下記を実施する。
** ① corder->left が指すドングルの in_use を true にする（mutex で守る）。
**    ※円卓にあるのはこれ一本のみ。mutex は握りっぱなしにしない。
** ② “<時刻> <id> has taken a dongle” というログを出力
** ③ stop=true になるまで、「stop を見る → まだ false なら少し待機 → また見る」
**    を繰り返す。※stop=true になったら抜ける。
** ④ ドングルの in_use を false に戻す（mutex で守る）。
** ⑤ NULL を返して終わる。 */
static void	*run_simulation_when_one_corder(t_corder *corder)
{
    // ①
	pthread_mutex_lock(&corder->left->mutex);
	corder->left->in_use = true;
	pthread_mutex_unlock(&corder->left->mutex);
    
    // ②
	pthread_mutex_lock(&corder->table->log_mutex);
	fprintf(stdout, "%lld ", get_time_ms() - corder->table->start_time);
	fprintf(stdout, "%ju ", corder->id);
	fprintf(stdout, "has taken a dongle\n");
	pthread_mutex_unlock(&corder->table->log_mutex);
    
    // ③
	while (!is_stopped(corder->table))
		usleep(500);
    
    // ④
	pthread_mutex_lock(&corder->left->mutex);
	corder->left->in_use = false;
	pthread_mutex_unlock(&corder->left->mutex);
    
    // ⑤
	return (NULL);
}

static void	*run_simulation_when_some_corders(t_corder *corder)
{
	t_dongle	*first;
	t_dongle	*second;

	while (!is_stopped(corder->table))
	{
		set_dongle_order(corder, &first, &second);

		if (!take_a_dongle(corder, first))
			break;
		if (!take_a_dongle(corder, second))
		{
			release_a_dongle(corder, first);
			break;
		}

		/* 以降の処理（コンパイル、ドングルを返す、デバッグ、リファクタ）
		/* ①コンパイル開始
		/* ・corder->mutex で鍵をかけて、last_compile_start を今の時刻にする（monitor も読む値やから鍵がいる）
		/* ・is compiling のログを出す
		/* time_to_compile ミリ秒待つ（README どおり 500µs ずつ刻んで、stop も確認する）
		/* ②コンパイル終了
		/* ・corder->mutex で鍵をかけて、compile_count を1増やす
		/* ③ドングルを2本とも返す
		/* ・release_a_dongle(corder, first) と release_a_dongle(corder, second)
		/* ・中では in_use = false、released_at を今の時刻、pthread_cond_broadcast で隣を起こす
		/* ④デバッグ
		/* ・is debugging のログを出して、time_to_debug ミリ秒待つ
		/* ⑤リファクタ
		/* ・is refactoring のログを出して、time_to_refactor ミリ秒待つ
		/* ・終わったら while の先頭に戻って、また2本取りにいく */
		
	}
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
			
	// corder の人数によって処理を分ける
	if (config->number_of_corders == 1)
		return (run_simulation_when_one_corder(corder));
	else
		return (run_simulation_when_some_corders(corder));
}
