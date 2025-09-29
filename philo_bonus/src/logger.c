/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 14:30:07 by adjeuken          #+#    #+#             */
/*   Updated: 2025/09/13 14:30:29 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	logger_drain_locked(t_logger *lg)
{
	int	i;
	int	idx;

	i = 0;
	while (i < lg->count)
	{
		idx = (lg->head + i) % lg->cap;
		free(lg->q[idx].msg);
		i++;
	}
	lg->count = 0;
	lg->head = lg->tail;
}

static int	logger_take_one(t_logger *lg, t_log_item *it)
{
	if (lg->count == 0)
		return (0);
	*it = lg->q[lg->head];
	lg->head = (lg->head + 1) % lg->cap;
	lg->count--;
	return (1);
}

void	*logger_thread(void *arg)
{
	t_logger	*lg;
	t_log_item	it;

	lg = (t_logger *)arg;
	while (1)
	{
		pthread_mutex_lock(&lg->mtx);
		if (lg->stop)
		{
			logger_drain_locked(lg);
			pthread_mutex_unlock(&lg->mtx);
			return (NULL);
		}
		if (!logger_take_one(lg, &it))
		{
			pthread_mutex_unlock(&lg->mtx);
			usleep(100);
			continue ;
		}
		pthread_mutex_unlock(&lg->mtx);
		if (!it.msg)
			continue ;
		write(1, it.msg, str_len(it.msg));
		free(it.msg);
	}
}

void	logger_stop_and_join(t_logger *lg)
{
	if (!lg)
		return ;
	pthread_mutex_lock(&lg->mtx);
	lg->stop = 1;
	pthread_mutex_unlock(&lg->mtx);
	pthread_join(lg->thread, NULL);
}

void	logger_destroy(t_logger *lg)
{
	int	i;
	int	idx;

	if (!lg)
		return ;
	if (lg->q)
	{
		i = 0;
		while (i < lg->count)
		{
			idx = (lg->head + i) % lg->cap;
			free(lg->q[idx].msg);
			i++;
		}
		free(lg->q);
		lg->q = NULL;
	}
	pthread_mutex_destroy(&lg->mtx);
}
