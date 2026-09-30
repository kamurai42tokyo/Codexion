/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kamurai <kamurai>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 01:07:57 by kamurai           #+#    #+#             */
/*   Updated: 2026/09/29 04:35:06 by kamurai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <stdint.h>
# include <stdbool.h>

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

uintmax_t	arg_to_num(char *str);

#endif