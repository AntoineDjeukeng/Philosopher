/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 16:23:43 by adjeuken          #+#    #+#             */
/*   Updated: 2025/08/30 18:15:00 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	start_threads(t_ctx *a)
{
	int	i;

	a->pp = (t_philo **)malloc(sizeof(*a->pp) * (size_t)a->rules.n);
	if (!a->pp)
		return (ft_puterr("alloc failed\n"), 1);
	a->mon = (t_monitor *)malloc(sizeof(*a->mon));
	if (!a->mon)
		return (free(a->pp), ft_puterr("alloc failed\n"), 1);
	pthread_create(&a->th_log, NULL, logger_thread, a->state.logger);
	i = 0;
	while (i < a->rules.n)
	{
		pthread_create(&a->threads[i], NULL, philo_run, &a->philos[i]);
		a->pp[i] = &a->philos[i];
		i++;
	}
	a->mon->philos = a->pp;
	a->mon->state = &a->state;
	pthread_create(&a->th_mon, NULL, monitor_thread, a->mon);
	return (0);
}

int	run_threads(t_ctx *a)
{
	int	i;

	if (start_threads(a))
		return (1);
	i = 0;
	while (i < a->rules.n)
	{
		pthread_join(a->threads[i], NULL);
		i++;
	}
	pthread_join(a->th_mon, NULL);
	pthread_mutex_lock(&a->state.logger->mx);
	a->state.logger->stop = 1;
	pthread_mutex_unlock(&a->state.logger->mx);
	pthread_join(a->th_log, NULL);
	free(a->mon);
	free(a->pp);
	return (0);
}
