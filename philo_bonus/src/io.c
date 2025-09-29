/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   io.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 14:29:24 by adjeuken          #+#    #+#             */
/*   Updated: 2025/09/13 14:29:25 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	str_len(const char *s)
{
	int	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

static void	putstr_s(char *s, int do_free)
{
	int	i;

	if (!s)
		return ;
	i = 0;
	while (s[i])
		i++;
	if (i > 0)
		write(1, s, i);
	if (do_free)
		free(s);
}

void	util_put_ms_id_msg(long long ms, long long id, const char *msg)
{
	putstr_s(ll_to_str(ms), 1);
	write(1, " ", 1);
	putstr_s(ll_to_str(id), 1);
	write(1, " ", 1);
	write(1, msg, str_len(msg));
}
