/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   take_dongle.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/25 13:52:21 by kamurai           #+#    #+#             */
/*   Updated: 2026/10/25 20:57:35 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

static void	submit_request(t_corder *corder, t_dongle *dongle)
{
	t_config	*config;
	t_request	*request;

	config = &(corder->table->config);
	
	pthread_mutex_lock(&(dongle->mutex));
	if (dongle->request_count >= 2)
		pthread_mutex_unlock(&(dongle->mutex));
	else
	{
		request = &(dongle->request_queue[dongle->request_count]);
		request->id = corder->id;
		request->deadline = corder->last_compile_start + config->time_to_burnout;
		request->arrival_order = dongle->next_arrival_order;
		dongle->next_arrival_order++;
		dongle->request_count++;
		pthread_mutex_unlock(&(dongle->mutex));
	}
}

/* ドングルの状況が変わるまで、鍵を外して１回だけ寝る。
** 寝る期間は、ドングルが cooldown 中かどうかで異なる。
** 【cooldown 中の場合】cooldown が終わるまでorもしくは合図があるまで。
** 【cooldown を既に終えている場合】合図があるまで。
** また、起きたら鍵をかけ直す。*/
static void	wait_dongle(t_corder *corder, t_dongle *dongle)
{
	long long		cooldown_end;
	struct timespec	wake_at;

	cooldown_end = dongle->released_at + corder->table->config.dongle_cooldown;

	if (!dongle->in_use && get_time_ms() < cooldown_end)
	{
		wake_at.tv_sec = cooldown_end / 1000;
		wake_at.tv_nsec = (cooldown_end % 1000) * 1000000;

		pthread_cond_timedwait(&(dongle->cond), &(dongle->mutex), &wake_at);
	}
	else
		pthread_cond_wait(&(dongle->cond), &(dongle->mutex));
}

// 申込書を消す（前に詰める）
static void	remove_request(t_corder *corder, t_dongle *dongle)
{
	if (dongle->request_queue[0].id == corder->id)
	{
		if (dongle->request_count == 2)
			dongle->request_queue[0] = dongle->request_queue[1];
		dongle->request_count--;
	}
	else if (dongle->request_queue[1].id == corder->id)
		dongle->request_count--;
}

// ドングル１つに対し「申請 → 待つ → 取る」を実施する。
bool	take_a_dongle(t_corder *corder, t_dongle *dongle)
{
	// 申請
	submit_request(corder, dongle);

	// 待つ
	pthread_mutex_lock(&(dongle->mutex));
	while (!is_my_turn(corder, dongle))
	{
		if (is_stopped(corder->table))
			return (pthread_mutex_unlock(&(dongle->mutex)), false);

		// ドングルの状況が変わるまで、鍵を外して１回だけ寝る
		wait_dongle(corder, dongle);
	}
		
	// 取る
	dongle->in_use = true;
	remove_request(corder, dongle);
	return (pthread_mutex_unlock(&(dongle->mutex)), true);
}
