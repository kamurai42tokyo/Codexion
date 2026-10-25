/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/25 13:28:35 by kamurai           #+#    #+#             */
/*   Updated: 2026/10/25 13:36:39 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

/* シミュレーションが終わるまで全corderを短い間隔で順番に見て回る。
** 「燃え尽き」もしくは「全員完了」の判定を実施する。
** 
** 1. 「今の時刻（last_compile_start）」が time_to_burnout 以上の corder が１人でも
**    おったら、burned out のログを出して stop を ture にする。
** 2. 全員の compile_count が number_of_compiles_required 以上になっとったら、
**    ログは出さずに step を ture にする。
** 3. stop を true にしたら、全ドングルの cond で寝とる corder スレッドを起こして、
**    終わらせる。*/ 
void    *monitor_routine(void *arg)
{
	
}
