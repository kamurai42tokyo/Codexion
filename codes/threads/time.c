/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 01:29:59 by kamurai           #+#    #+#             */
/*   Updated: 2026/11/02 20:58:06 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"
#include <stdarg.h>

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

// 500µs ずつ、合計 ms ミリ秒寝る
void    sleep_in_500us_steps(t_table *table, long long ms)
{
    long long   start;

    start = get_time_ms();
    while (get_time_ms() - start < ms)
    {
        if (is_stopped(table))
            return ;
        usleep(500);
    }
}
