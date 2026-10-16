/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 01:07:57 by kamurai           #+#    #+#             */
/*   Updated: 2026/10/13 16:47:37 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <stdint.h>
# include <stdlib.h>
# include <stdarg.h>
# include <string.h>
# include <stdbool.h>
# include <pthread.h>
# include <sys/time.h>

typedef struct s_table	t_table;

/*
** t_config: コマンドライン引数8つから読み取ったシミュレーションの設定。
** 個数: 1個。t_table の config に埋め込む。
** 共有: 全スレッドが読む。main が設定した後は誰も書き換えないため鍵は不要。
*/
typedef struct s_config
{
	uintmax_t	number_of_coders;				// corder の人数
	uintmax_t	time_to_burnout;				// 燃え尽きまでの時間(ms)
	uintmax_t	time_to_compile;				// コンパイル時間(ms)
	uintmax_t	time_to_debug;					// デバッグ時間(ms)
	uintmax_t	time_to_refactor;				// リファクタ時間(ms)
	uintmax_t	number_of_compiles_required;	// 終了に必要な回数
	uintmax_t	dongle_cooldown;				// 再利用までの時間(ms)
	char		*scheduler;						// "fifo" か "edf"
}	t_config;

/*
** t_request: corder がドングルの待ち行列に出す申込書1枚。
** 個数: ドングル1本につき最大2枚。t_dongle の queue に置く。
** 共有: t_dongle の一部なので、そのドングルの mutex で守る。
*/
typedef struct s_request
{
	uintmax_t	id;								// 申し込んだ corder の番号
	long long	deadline;						// 燃え尽き期限(ms)（edf用）
	uintmax_t	arrival_order;					// 到着順で割り振られる番号（fifo用）
}	t_request;

/*
** t_dongle: 円卓の上の USB ドングル1本。
** 個数: number_of_coders 本。t_table の dongle 配列に置く。
** 共有: 左右の corder 2人が読み書きする。全メンバを mutex で守る。
*/
typedef struct s_dongle
{
	bool			in_use;						// 使用中なら true
	long long		released_at;				// 最後に返された時刻(ms)
	t_request		request_queue[2];			// 待ちの申込書(最大2件)
	int				request_count;				// queue の件数(0〜2)
	uintmax_t		next_arrival_order;			// 次に配る到着番号
	pthread_mutex_t	mutex;						// このドングルの状態と待ち行列を守る鍵
	pthread_cond_t	cond;						// 待つ corder を起こす合図
}	t_dongle;

/*
** t_corder: 円卓に座る corder 1人(スレッド1本につき1個)。
** 個数: number_of_coders 個。t_table の corders 配列に置く。
** 共有: その corder のスレッドが書き、monitor スレッドが読む(守る鍵は未定)。
*/
typedef struct s_corder
{
	uintmax_t		id;							// corder の番号(1始まり)
	long long		last_compile_start;			// 最後のコンパイル開始時刻(ms)
	uintmax_t		compile_count;				// コンパイルを終えた回数
	pthread_mutex_t	mutex;						// 上の値を守る鍵
	t_table			*table;						// 円卓の住所
	t_dongle		*left;						// 左のドングル
	t_dongle		*right;						// 右のドングル
}	t_corder;

/*
** t_table: corder とドングルが囲む円卓。
** 個数: 1個。run_simulations が用意する。
** 共有: 全スレッドが読む。stop だけは書き換わるため stop_mutex で守る。
*/
struct s_table
{
	t_config		config;						// 引数の設定(読み取り専用)
	t_dongle		*dongles;					// ドングルの配列
	t_corder		*corders;					// corder の配列
	long long		start_time;					// シミュレーション開始時刻
	bool			stop;						// true で全員停止
	pthread_mutex_t	stop_mutex;					// stop を守る鍵
	pthread_mutex_t	log_mutex;					// ログ出力を守る鍵
};

bool		is_scheduler(char *str);
bool		set_num(char *str, uintmax_t *num);
bool		set_scheduler(char *src, char *dst);
void		free_all(int count, ...);
void		free_table(t_table *table);
void    	destroy_dongle_mutexes(t_dongle *dongles, uintmax_t count);
void    	destroy_corder_mutexes(t_corder *corders, uintmax_t count);
void		destroy_table_mutexes(t_table *table);
bool		run_simulations(t_config data);
bool    	set_table(t_table **table, t_config config);
long long	get_time_ms(void);
void		*run_simulation(void *arg);
void 		*monitor_routine(void *arg);

#endif