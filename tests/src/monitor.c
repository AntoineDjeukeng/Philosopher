/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 16:41:16 by adjeuken          #+#    #+#             */
/*   Updated: 2025/08/30 18:33:10 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	mon_all_full(t_monitor *ma, int *done)
{
	int	i;
	int	full;
	int	n;

	i = 0;
	full = 0;
	n = ma->state->rules->n;
	while (i < n)
	{
		if (!done[i] && (int)ma->philos[i]->meals >= ma->state->rules->t_meals)
			done[i] = 1;
		full += done[i];
		i++;
	}
	if (full == n)
		return (1);
	return (0);
}

static int	mon_check_dead(t_monitor *ma, int *vid, int64_t *rel)
{
	int		i;
	int		n;
	int64_t	t;
	int64_t	last;
	t_rules	*rules;

	rules = ma->state->rules;
	n = rules->n;
	i = 0;
	t = now_ms();
	while (i < n)
	{
		pthread_mutex_lock(&ma->state->state_mx);
		last = ma->philos[i]->last_meal_ms;
		pthread_mutex_unlock(&ma->state->state_mx);
		if (t - last > rules->t_die)
		{
			*vid = ma->philos[i]->id;
			*rel = (last + rules->t_die) - ma->state->logger->start_ms;
			return (1);
		}
		i++;
	}
	return (0);
}

static int	monitor_loop(t_monitor *ma, int *done)
{
	int		vid;
	int64_t	rel;

	while (!stop_snapshot(ma->state))
	{
		if (done && mon_all_full(ma, done))
		{
			pthread_mutex_lock(&ma->state->state_mx);
			ma->state->stop = 1;
			pthread_mutex_unlock(&ma->state->state_mx);
			return (0);
		}
		if (mon_check_dead(ma, &vid, &rel))
		{
			pthread_mutex_lock(&ma->state->state_mx);
			ma->state->stop = 1;
			pthread_mutex_unlock(&ma->state->state_mx);
			logger_kill_with_death(ma->state->logger, vid, rel);
			return (1);
		}
		usleep(1000);
	}
	return (0);
}

void	*monitor_thread(void *arg)
{
	t_monitor	*ma;
	int			*done;
	int			r;

	ma = (t_monitor *)arg;
	done = NULL;
	if (ma->state->rules->t_meals > 0)
	{
		done = (int *)ft_calloc(ma->state->rules->n, sizeof(int));
		if (!done)
			return (NULL);
	}
	r = monitor_loop(ma, done);
	if (!r)
	{
		pthread_mutex_lock(&ma->state->logger->mx);
		ma->state->logger->stop = 1;
		pthread_mutex_unlock(&ma->state->logger->mx);
	}
	free(done);
	return (NULL);
}
