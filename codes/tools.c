/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 01:29:59 by kamurai           #+#    #+#             */
/*   Updated: 2026/10/25 20:19:10 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdarg.h>

bool	is_scheduler(char *str)
{
	if (strcmp(str, "fifo") == 0 || strcmp(str, "edf") == 0)
		return (true);
	return (false);
}

bool	set_num(char *str, uintmax_t *num)
{
	uintmax_t	result;
	uintmax_t	i;

	if (str[0] == '\0')
		return (fprintf(stderr, "Error: argument is empty\n"), false);
	result = 0;
	i = 0;
	while (str[i])
	{
		if (str[i] >= '0' && str[i] <= '9')
		{
			if (result > (UINTMAX_MAX - (str[i] - '0')) / 10)
				return (
					fprintf(stderr, "Error: argument is too large\n"), false);
			else
				result = result * 10 + (str[i] - '0');
		}
		else
			return (fprintf(stderr, "Error: argument is not digit\n"), false);
		i++;
	}
	return (*num = result, true);
}

bool	set_scheduler(char *src, char *dst)
{
	size_t	i;

	i = 0;
	while (src[i])
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
	return (true);
}

/* 呼び出したスレッドに、1970年1月1日0時0分0秒（UTC）から
** 呼び出した瞬間までの経過時間を ms 単位の整数で返す関数 */
long long	get_time_ms(void)
{
	/* 1970年1月1日0時0分0秒（UTC）から gettimeofday() を
	** 呼んだ瞬間までの経過時間を受け取るための入れ物。*/
	struct timeval	tv;
	
	gettimeofday(&tv, NULL);
	return ((long long)tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void	set_dongle_order(t_corder *corder, t_dongle **first, t_dongle **second)
{
	if (corder->id % 2 == 0)
	{
		*first = corder->right;
		*second = corder->left;
	}
	else
	{
		*first = corder->left;
		*second = corder->right;
	}
}
