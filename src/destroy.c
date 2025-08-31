/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   destroy.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 16:44:08 by adjeuken          #+#    #+#             */
/*   Updated: 2025/08/30 16:45:38 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	destroy_all(t_rules *rules, t_state *state, t_logger *lg)
{
	int			i;
	t_logmsg	*m;

	i = 0;
	while (i < rules->n)
	{
		pthread_mutex_destroy(&rules->forks[i]);
		i++;
	}
	free(rules->forks);
	pthread_mutex_destroy(&state->state_mx);
	while (lg->head)
	{
		m = lg->head;
		lg->head = m->next;
		free(m);
	}
	pthread_mutex_destroy(&lg->mx);
}
