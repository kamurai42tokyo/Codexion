/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 20:26:40 by kamurai           #+#    #+#             */
/*   Updated: 2026/11/02 14:51:55 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

// coder の id が奇数の場合と偶数の場合とで、左右のドングルの優先順位を変える
void	set_dongle_order(t_coder *coder, t_dongle **first, t_dongle **second)
{
	if (coder->id % 2 == 0)
	{
		*first = coder->right;
		*second = coder->left;
	}
	else
	{
		*first = coder->left;
		*second = coder->right;
	}
}

/* coder が１人（＝ dongle が１つ）の場合の処理。下記を実施する。
** ① coder->left が指すドングルの in_use を true にする（mutex で守る）。
**    ※円卓にあるのはこれ一本のみ。mutex は握りっぱなしにしない。
** ② “<時刻> <id> has taken a dongle” というログを出力
** ③ stop=true になるまで、「stop を見る → まだ false なら少し待機 → また見る」
**    を繰り返す。※stop=true になったら抜ける。
** ④ ドングルの in_use を false に戻す（mutex で守る）。
** ⑤ NULL を返して終わる。 */
static void	*run_simulation_when_one_coder(t_coder *coder)
{
    // ①
	pthread_mutex_lock(&coder->left->mutex);
	coder->left->in_use = true;
	pthread_mutex_unlock(&coder->left->mutex);
    
    // ②
	print_log("take dongle", coder);
    
    // ③
	while (!is_stopped(coder->table))
		usleep(500);
    
    // ④
	pthread_mutex_lock(&coder->left->mutex);
	coder->left->in_use = false;
	pthread_mutex_unlock(&coder->left->mutex);
    
    // ⑤
	return (NULL);
}

static void	*run_simulation_when_some_coders(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	while (!is_stopped(coder->table))
	{
		// coder の id が奇数の場合と偶数の場合とで、左右のドングルの優先順位を変える
		set_dongle_order(coder, &first, &second);

		// ドングルを２本取る
		if (!take_dongles(coder, first, second))
			break;

		// コンパイリング
		compiling(coder);

		// ドングルを2本とも返す
		release_dongles(first, second);

		// デバッキング
		debugging(coder);
		
		// リファクタ
		refactoring(coder);
	}
	return (NULL);
}

/* coder１人のスレッドが、シミュレーション開始からstop=trueになるまで下記の処理を繰り返す。
** 
** 1. 左右のドングルへ申込書（t_request）を出して、scheduler（fifoかedf）の順番が
**    来るまで待ち、２本を取る。
**    cooldown（返されてからdongle_cooldown 秒）が過ぎるまでは取らない。
**    １本取るたびに has taken a dongle のログ出し、time_to_compile秒持つ。
** 2. last_compile_start を今の時刻にして、 is compiling のログを出し、
**    time_to_compiling 秒待つ。
** 3. compile_count を１増やして、２本のドングルを返す。返すときは released_at に
**    今の時刻を入れて、待っとる隣の coder を cond で起こす。
** 4. is debugging のログ出して、time_to_debug秒 待つ。
** 5. is refactoring のログ出して、time_to_refactor秒 待つ。*/
void	*run_simulation(void *arg)
{
	t_coder    *coder;
    t_config	*config;

	coder = arg;
	config = &coder->table->config;
			
	// coder の人数によって処理を分ける
	if (config->number_of_coders == 1)
		return (run_simulation_when_one_coder(coder));
	else
		return (run_simulation_when_some_coders(coder));
}
