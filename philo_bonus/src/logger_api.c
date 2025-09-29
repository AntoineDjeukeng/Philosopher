/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger_api.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken <adjeuken@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 14:29:49 by adjeuken          #+#    #+#             */
/*   Updated: 2025/09/29 19:38:21 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <fcntl.h>

int	logger_init(t_logger *lg, int capacity)
{
	char	name[32];

	if (!lg || capacity <= 0)
		return (0);
	lg->sem = NULL;
	sem_name_from_ptr(name, 'l', (uintptr_t)lg, -1);
	sem_unlink(name);
	lg->sem = sem_open(name, O_CREAT | O_EXCL, 0600, 1);
	if (lg->sem == SEM_FAILED)
		return (0);
	sem_unlink(name);
	lg->q = (t_log_item *)malloc(sizeof(t_log_item) * (size_t)capacity);
	if (!lg->q)
	{
		sem_close(lg->sem);
		lg->sem = NULL;
		return (0);
	}
	lg->cap = capacity;
	lg->head = 0;
	lg->tail = 0;
	lg->count = 0;
	lg->stop = 0;
	return (1);
}

static int	logger_try_enqueue(t_logger *lg, char *msg)
{
	if (!lg || !lg->sem || lg->sem == SEM_FAILED)
		return (0);
	sem_wait(lg->sem);
	if (lg->stop)
	{
		sem_post(lg->sem);
		return (-1);
	}
	if (lg->count < lg->cap)
	{
		lg->q[lg->tail].msg = msg;
		lg->tail = (lg->tail + 1) % lg->cap;
		lg->count++;
		sem_post(lg->sem);
		return (1);
	}
	sem_post(lg->sem);
	return (0);
}

int	logger_enqueue(t_logger *lg, char *msg)
{
	int	r;

	if (!lg)
	{
		free(msg);
		return (0);
	}
	if (!lg->sem || lg->sem == SEM_FAILED)
	{
		free(msg);
		return (0);
	}
	while (1)
	{
		r = logger_try_enqueue(lg, msg);
		if (r == 1)
			return (1);
		if (r == -1)
		{
			free(msg);
			return (0);
		}
		usleep(100);
	}
}

int	logger_start(t_logger *lg)
{
	if (!lg)
		return (0);
	if (pthread_create(&lg->thread, NULL, logger_thread, lg) != 0)
		return (0);
	return (1);
}

int	logger_take_one(t_logger *lg, t_log_item *it)
{
	if (lg->count == 0)
		return (0);
	*it = lg->q[lg->head];
	lg->head = (lg->head + 1) % lg->cap;
	lg->count--;
	return (1);
}
