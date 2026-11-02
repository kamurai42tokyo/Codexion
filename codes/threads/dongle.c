/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/25 13:52:21 by kamurai           #+#    #+#             */
/*   Updated: 2026/11/02 15:20:22 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

/* ドングルの状況が変わるまで、鍵を外して１回だけ寝る。
** 寝る期間は、ドングルが cooldown 中かどうかで異なる。
** 【cooldown 中の場合】cooldown が終わるまでorもしくは合図があるまで。
** 【cooldown を既に終えている場合】合図があるまで。
** また、起きたら鍵をかけ直す。*/
static void	wait_dongle(t_coder *coder, t_dongle *dongle)
{
	long long		cooldown_end;
	struct timespec	wake_at;

	cooldown_end = dongle->released_at + coder->table->config.dongle_cooldown;

	if (!dongle->in_use && get_time_ms() < cooldown_end)
	{
		wake_at.tv_sec = cooldown_end / 1000;
		wake_at.tv_nsec = (cooldown_end % 1000) * 1000000;

		pthread_cond_timedwait(&(dongle->cond), &(dongle->mutex), &wake_at);
	}
	else
		pthread_cond_wait(&(dongle->cond), &(dongle->mutex));
}

/* ドングルを1本だけ返す */
void    release_a_dongle(t_dongle *dongle)
{
    pthread_mutex_lock(&(dongle->mutex));
    dongle->in_use = false;
    dongle->released_at = get_time_ms();
    pthread_cond_broadcast(&(dongle->cond));
	pthread_mutex_unlock(&(dongle->mutex));
}

/* ドングルを2本とも返す */
void    release_dongles(t_dongle *dongle1, t_dongle *dongle2)
{
    release_a_dongle(dongle1);
    release_a_dongle(dongle2);
}

// ドングル１つに対し「申請 → 待つ → 取る」を実施する。
bool	take_a_dongle(t_coder *coder, t_dongle *dongle)
{
	t_request	new_request;
	
	pthread_mutex_lock(&(dongle->mutex));
	
	// 申請書の作成
	new_request = make_request(coder, dongle);

	// 申請書の提出
	push_request(new_request, dongle, coder->table->config.scheduler);

	// 待つ
	while (!is_my_turn(coder, dongle))
	{
		if (is_stopped(coder->table))
			return (pthread_mutex_unlock(&(dongle->mutex)), false);

		// ドングルの状況が変わるまで、鍵を外して１回だけ寝る
		wait_dongle(coder, dongle);
	}
		
	// 取る
	dongle->in_use = true;
	print_log("take dongle", coder);
	remove_request(coder, dongle);
	return (pthread_mutex_unlock(&(dongle->mutex)), true);
}

// 左右のドングルを取得できたら true、できなければ false を返す。
bool	take_dongles(t_coder *coder, t_dongle *first, t_dongle *second)
{
	if (!take_a_dongle(coder, first))
		return (false);
	if (!take_a_dongle(coder, second))
	{
		release_a_dongle(first);
		return (false);
	}
	return (true);
}
