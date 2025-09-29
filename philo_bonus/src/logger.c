/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 14:30:07 by adjeuken          #+#    #+#             */
/*   Updated: 2025/09/29 23:32:36 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	logger_drain_locked(t_logger *lg)
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

static void	*logger_drain_and_unlock(t_logger *lg)
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
	sem_post(lg->sem);
	return (NULL);
}

void	*logger_thread(void *arg)
{
	t_logger	*lg;
	t_log_item	it;

	lg = (t_logger *)arg;
	while (1)
	{
		if (!lg->sem || lg->sem == SEM_FAILED)
			return (NULL);
		sem_wait(lg->sem);
		if (lg->stop)
			return (logger_drain_and_unlock(lg));
		if (!logger_take_one(lg, &it))
		{
			sem_post(lg->sem);
			usleep(100);
			continue ;
		}
		sem_post(lg->sem);
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
	if (lg->sem && lg->sem != SEM_FAILED)
	{
		sem_wait(lg->sem);
		lg->stop = 1;
		sem_post(lg->sem);
		pthread_join(lg->thread, NULL);
	}
	else
		lg->stop = 1;
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
	if (lg->sem && lg->sem != SEM_FAILED)
		sem_close(lg->sem);
	lg->sem = NULL;
}
