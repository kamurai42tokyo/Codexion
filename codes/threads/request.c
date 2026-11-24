/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/01 21:29:22 by kamurai           #+#    #+#             */
/*   Updated: 2026/11/05 17:49:21 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

/*
** Creates a request holding the coder's id and burnout deadline.
*/
t_request	make_request(t_coder *coder)
{
	t_config	config;
	long long	deadline;
	t_request	request;

	config = coder->table->config;
	deadline = coder->last_compile_start + config.time_to_burnout;
	request.id = coder->id;
	request.deadline = deadline;
	return (request);
}

/*
** Compares two requests for edf.
** 1. The earlier deadline comes first
** 2. On a tie, odd ids come first
** 3. If still tied, the smaller id comes first
** Return: true if new_request has higher priority, false otherwise
*/
static bool	is_higher(t_request new_request, t_request queued_request)
{
	if (new_request.deadline != queued_request.deadline)
		return (new_request.deadline < queued_request.deadline);
	else if (new_request.id % 2 != queued_request.id % 2)
		return (new_request.id % 2 == 1);
	else
		return (new_request.id < queued_request.id);
}

/*
** Appends the request to the end of request_queue.
** With edf, reorders the queue by earliest deadline.
** On a tie, odd ids come first, then the smaller id (see is_higher).
*/
void	push_request(t_request new_request, t_dongle *dongle, char *scheduler)
{
	t_request	*req_queue;
	t_request	tmp;
	int			i;

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
		while (i && is_higher(req_queue[i], req_queue[(i - 1) / 2]))
		{
			tmp = req_queue[i];
			req_queue[i] = req_queue[(i - 1) / 2];
			req_queue[(i - 1) / 2] = tmp;
			i = (i - 1) / 2;
		}
	}
}

/*
** Removes the first request and shifts the rest forward.
*/
void	remove_request(t_dongle *dongle)
{
	if (dongle->request_count == 2)
		dongle->request_queue[0] = dongle->request_queue[1];
	dongle->request_count--;
}
