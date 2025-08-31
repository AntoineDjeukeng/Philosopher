/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 16:43:39 by adjeuken          #+#    #+#             */
/*   Updated: 2025/08/30 16:44:02 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	init_forks(t_rules *rules)
{
	int	i;

	rules->forks = (pthread_mutex_t *)malloc(sizeof(*rules->forks)
			* (size_t)rules->n);
	if (!rules->forks)
		return (0);
	i = 0;
	while (i < rules->n)
	{
		pthread_mutex_init(&rules->forks[i], NULL);
		i++;
	}
	return (1);
}

int	init_state(t_state *state, t_rules *rules, t_logger *lg)
{
	state->rules = rules;
	state->logger = lg;
	state->stop = 0;
	if (pthread_mutex_init(&state->state_mx, NULL) != 0)
		return (0);
	return (1);
}

void	init_philos(t_philo *ph, t_state *state)
{
	int		i;
	int64_t	start;

	start = state->logger->start_ms;
	i = 0;
	while (i < state->rules->n)
	{
		ph[i].id = i + 1;
		ph[i].left = i;
		ph[i].right = (i + 1) % state->rules->n;
		ph[i].meals = 0;
		ph[i].last_meal_ms = start;
		ph[i].state = state;
		i++;
	}
}

void	logger_init(t_logger *lg)
{
	pthread_mutex_init(&lg->mx, NULL);
	lg->head = NULL;
	lg->tail = NULL;
	lg->stop = 0;
	lg->killed = 0;
	lg->start_ms = now_ms();
	if (lg->start_ms < 0)
		lg->start_ms = 0;
}
