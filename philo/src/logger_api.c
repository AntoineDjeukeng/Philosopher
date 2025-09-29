/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger_api.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 14:29:49 by adjeuken          #+#    #+#             */
/*   Updated: 2025/09/13 14:29:50 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*logger_thread(void *arg);

int	logger_init(t_logger *lg, int capacity)
{
	if (!lg || capacity <= 0)
		return (0);
	if (pthread_mutex_init(&lg->mtx, NULL) != 0)
		return (0);
	lg->q = (t_log_item *)malloc(sizeof(t_log_item) * (size_t)capacity);
	if (!lg->q)
	{
		pthread_mutex_destroy(&lg->mtx);
		return (0);
	}
	lg->cap = capacity;
	lg->head = 0;
	lg->tail = 0;
	lg->count = 0;
	lg->stop = 0;
	return (1);
}

int	logger_enqueue(t_logger *lg, char *msg)
{
	if (!lg)
		return (0);
	while (1)
	{
		pthread_mutex_lock(&lg->mtx);
		if (lg->stop)
		{
			pthread_mutex_unlock(&lg->mtx);
			if (msg)
				free(msg);
			return (0);
		}
		if (lg->count < lg->cap)
		{
			lg->q[lg->tail].msg = msg;
			lg->tail = (lg->tail + 1) % lg->cap;
			lg->count++;
			pthread_mutex_unlock(&lg->mtx);
			return (1);
		}
		pthread_mutex_unlock(&lg->mtx);
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
