/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 01:07:57 by kamurai           #+#    #+#             */
/*   Updated: 2026/10/01 06:51:17 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <stdint.h>
# include <stdlib.h>
# include <stdbool.h>
# include <pthread.h>

typedef struct s_data
{
	uintmax_t	number_of_coders;
	uintmax_t	time_to_burnout;
	uintmax_t	time_to_compile;
	uintmax_t	time_to_debug;
	uintmax_t	time_to_refactor;
	uintmax_t	number_of_compiles_required;
	uintmax_t	dongle_cooldown;
	char		*scheduler;
}		t_data;

typedef struct s_corder
{
	
}	t_corder;

bool	is_scheduler(char *str);
bool	set_num(char *str, uintmax_t *num);

#endif