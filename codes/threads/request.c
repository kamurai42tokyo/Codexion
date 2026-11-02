/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/01 21:29:22 by kamurai           #+#    #+#             */
/*   Updated: 2026/11/02 15:04:11 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

// ドングルへの申請を作成する。
t_request	make_request(t_coder *coder, t_dongle *dongle)
{
	t_config    config;
	long long	deadline;
	t_request	request;

	config = coder->table->config;
	deadline = coder->last_compile_start + config.time_to_burnout;
    request.id = coder->id;
    request.deadline = deadline;
    request.arrival_order = dongle->next_arrival_order;
    dongle->next_arrival_order++;
    return (request);
}

// 申請書の提出（申請書を request_queue の末尾に入れて上へ移す。）
void    push_request(t_request new_request, t_dongle *dongle, char *scheduler)
{
    t_request   *req_queue;
    t_request   tmp;
    int         i;

    if (dongle->request_count >= 2)
        return ;

    dongle->request_queue[dongle->request_count] = new_request;
    dongle->request_count++;
    if (dongle->request_count == 1)
        return ;

    req_queue = dongle->request_queue;
    i = dongle->request_count - 1;
    if (strcmp(scheduler, "edf") == 0)
    {
        while (i && req_queue[i].deadline < req_queue[(i - 1) / 2].deadline)
        {
            tmp = req_queue[i];
            req_queue[i] = req_queue[(i - 1) / 2];
            req_queue[(i - 1) / 2] = tmp;
            i = (i - 1) / 2;
        }
    }
}

// 申込書を消す（前に詰める）
void	remove_request(t_coder *coder, t_dongle *dongle)
{
	if (dongle->request_queue[0].id == coder->id)
	{
		if (dongle->request_count == 2)
			dongle->request_queue[0] = dongle->request_queue[1];
		dongle->request_count--;
	}
	else if (dongle->request_queue[1].id == coder->id)
		dongle->request_count--;
}

// request_queue のうち deadline が最も早い id を返す
uintmax_t	get_earliest_deadline_id(t_dongle *dongle)
{
	if (dongle->request_count == 1)
		return (dongle->request_queue[0].id);
	if (dongle->request_queue[0].deadline <= dongle->request_queue[1].deadline)
		return (dongle->request_queue[0].id);
	else
		return (dongle->request_queue[1].id);
}
