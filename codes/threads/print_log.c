/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_log.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 16:22:11 by kamurai           #+#    #+#             */
/*   Updated: 2026/11/01 16:19:04 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

void    print_log(char *command, t_coder *coder)
{
    char    *msg;

    if (strcmp(command, "take dongle") == 0)
        msg = "has taken a dongle";
    else if (strcmp(command, "compiling") == 0)
        msg = "is compiling";
    else if (strcmp(command, "debugging") == 0)
        msg = "is debugging";
    else if (strcmp(command, "refactoring") == 0)
        msg = "is refactoring";
    else if (strcmp(command, "burn out") == 0)
        msg = "burned out";
    else
        return ((void)fprintf(stderr, "print_log: unkown command\n"));
    pthread_mutex_lock(&(coder->table->log_mutex));
    if (is_stopped(coder->table))
        return ((void)pthread_mutex_unlock(&(coder->table->log_mutex)));
    if (strcmp(command, "burn out") == 0)
        stop_simulation(coder->table);
    fprintf(stdout, "%lld ", get_time_ms() - coder->table->start_time);
    fprintf(stdout, "%ju ", coder->id);
    fprintf(stdout, "%s\n", msg);
    pthread_mutex_unlock(&(coder->table->log_mutex));
}
