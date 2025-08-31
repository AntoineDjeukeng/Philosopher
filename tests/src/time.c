/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 16:21:54 by adjeuken          #+#    #+#             */
/*   Updated: 2025/08/30 18:42:35 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int64_t	now_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) != 0)
		return (0);
	return (((int64_t)tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

void	precise_sleep_ms(volatile int *stop_ptr, pthread_mutex_t *stop_mx,
		int ms)
{
	int64_t	end;
	int		hit;

	end = now_ms() + ms;
	while (now_ms() < end)
	{
		hit = 0;
		if (stop_mx)
		{
			pthread_mutex_lock(stop_mx);
			hit = *stop_ptr;
			pthread_mutex_unlock(stop_mx);
		}
		else if (stop_ptr)
			hit = *stop_ptr;
		if (hit)
			break ;
		usleep(1000);
	}
}

int	stop_snapshot(t_state *sh)
{
	int	v;
	int	r;

	pthread_mutex_lock(&sh->state_mx);
	v = sh->stop;
	pthread_mutex_unlock(&sh->state_mx);
	r = 0;
	if (v)
		r = 1;
	return (r);
}

void	wait_until_stop(t_state *sh)
{
	int	v;

	while (1)
	{
		pthread_mutex_lock(&sh->state_mx);
		v = sh->stop;
		pthread_mutex_unlock(&sh->state_mx);
		if (v)
			break ;
		usleep(1000);
	}
}
