/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 01:07:57 by kamurai           #+#    #+#             */
/*   Updated: 2026/11/24 19:46:17 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdarg.h>
# include <stdbool.h>
# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>
# include <limits.h>

typedef struct s_table	t_table;

/*
** t_config: Simulation settings read from the 8 command line arguments.
** Count: 1. Embedded in t_table as config.
** Shared: Read by all threads. Never modified after main sets it,
**         so no lock is needed.
** Members:
**   number_of_coders            : Number of coders
**   time_to_burnout             : Time until burnout (ms)
**   time_to_compile             : Compile time (ms)
**   time_to_debug               : Debug time (ms)
**   time_to_refactor            : Refactor time (ms)
**   number_of_compiles_required : Compiles needed to finish
**   dongle_cooldown             : Time before reuse (ms)
**   scheduler                   : "fifo" or "edf"
*/
typedef struct s_config
{
	int			number_of_coders;	/* 人数: 1〜INT_MAX */
	long long	time_to_burnout;	/* ms: 時刻に足すため */
	long long	time_to_compile;	/* ms: 時刻と同じ型 */
	long long	time_to_debug;		/* ms: 時刻と同じ型 */
	long long	time_to_refactor;	/* ms: 時刻と同じ型 */
	int			number_of_compiles_required;	/* 回数: 0〜INT_MAX */
	long long	dongle_cooldown;	/* ms: 経過時間と比べるため */
	char		*scheduler;			/* "fifo" か "edf" */
}						t_config;

/*
** t_request: One request a coder puts in a dongle's waiting queue.
** Count: Up to 2 per dongle. Stored in t_dongle's request_queue.
** Shared: Part of t_dongle, so guarded by that dongle's mutex.
** Members:
**   id       : Id of the requesting coder
**   deadline : Burnout deadline (ms, for edf)
*/
typedef struct s_request
{
	int			id;			/* coder番号: 1〜number_of_coders */
	long long	deadline;	/* 時刻: 1970年からのms, INT_MAXを超える */
}						t_request;

/*
** t_dongle: One USB dongle on the round table.
** Count: number_of_coders. Stored in t_table's dongles array.
** Shared: Read and written by the 2 adjacent coders.
**         All members are guarded by mutex.
** Members:
**   in_use        : true while in use
**   released_at   : Last release time (ms)
**   request_queue : Waiting requests (max 2)
**   request_count : Number of requests (0-2)
**   mutex         : Guards state and queue
*/
typedef struct s_dongle
{
	bool			in_use;				/* 状態が2つだけ */
	long long		released_at;		/* 時刻: 1970年からのms */
	t_request		request_queue[2];	/* 待つcoderは最大2人 */
	int				request_count;		/* 小さい個数: 0〜2 */
	pthread_mutex_t	mutex;				/* 課題で必須 */
}						t_dongle;

/*
** t_coder: One coder at the round table (one per thread).
** Count: number_of_coders. Stored in t_table's coders array.
** Shared: Written by its own thread and read by the monitor thread.
**         last_compile_start and compile_count are guarded by mutex.
** Members:
**   id                 : Coder id (starts at 1)
**   last_compile_start : Last compile start (ms)
**   compile_count      : Number of compiles done
**   mutex              : Guards the values above
**   table              : Pointer to the table
**   right              : Right dongle
**   left               : Left dongle
*/
typedef struct s_coder
{
	int				id;					/* coder番号: 1〜N */
	long long		last_compile_start;	/* 時刻: 1970年からのms */
	int				compile_count;		/* 回数: intの値と比べるため */
	pthread_mutex_t	mutex;				/* 上の2つを守る */
	t_table			*table;				/* 共有するのでポインタ */
	t_dongle		*right;				/* 隣のcoderと共有 */
	t_dongle		*left;				/* 隣のcoderと共有 */
}						t_coder;

/*
** t_table: The round table shared by coders and dongles.
** Count: 1. Prepared by run_simulations.
** Shared: Read by all threads. Only stop changes,
**         so it is guarded by stop_mutex.
** Members:
**   config     : Settings (read-only)
**   dongles    : Array of dongles
**   coders     : Array of coders
**   start_time : Simulation start time (ms)
**   stop       : true stops everyone
**   stop_mutex : Guards stop
**   log_mutex  : Guards log output
*/
typedef struct s_table
{
	t_config		config;		/* 1回コピーして読むだけ */
	t_dongle		*dongles;	/* mallocしたN個の配列 */
	t_coder			*coders;	/* mallocしたN個の配列 */
	long long		start_time;	/* 時刻: 1970年からのms */
	bool			stop;		/* 状態が2つだけ */
	pthread_mutex_t	stop_mutex;	/* 課題で必須 */
	pthread_mutex_t	log_mutex;	/* 課題で必須 */
}						t_table;

void					free_all(int count, ...);
void					free_table(t_table *table);
void					destroy_dongle_mutexes(t_dongle *dongles,
							int count);
void					destroy_coder_mutexes(t_coder *coders, int count);
void					destroy_table_mutexes(t_table *table);
bool					run_simulations(t_config data);
bool					set_table(t_table **table, t_config config);
long long				get_time_ms(void);
void					*run_simulation(void *arg);
void					*monitor_routine(void *arg);
t_config				*set_data(char **argv);
void					print_log(char *command, t_coder *coder);
bool					is_stopped(t_table *table);
void					stop_simulation(t_table *table);
t_request				make_request(t_coder *coder);
void					push_request(t_request new_request, t_dongle *dongle,
							char *scheduler);
void					remove_request(t_dongle *dongle);
bool					is_my_turn(t_coder *coder, t_dongle *dongle);
bool					take_dongles(t_coder *coder, t_dongle *first,
							t_dongle *second);
void					release_dongles(t_dongle *dongle1, t_dongle *dongle2);
void					compiling(t_coder *coder);
void					debugging(t_coder *coder);
void					refactoring(t_coder *coder);
void					sleep_in_500us_steps(t_table *table, long long ms);

#endif