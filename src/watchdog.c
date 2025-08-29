/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   watchdog.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 00:00:00 by yourlogin         #+#    #+#             */
/*   Updated: 2025/08/29 10:29:36 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <time.h>

/* --- helper 1: sleep ~1 ms --- */
static void	sleep_1ms(void)
{
	struct timespec	ts;

	ts.tv_sec = 0;
	ts.tv_nsec = 1 * 1000 * 1000;
	nanosleep(&ts, NULL);
}

/* --- helper 2: print death once, under locks --- */
static void	announce_death(t_shared *s, int id, long now)
{
	long	ts;

	pthread_mutex_lock(&s->print);
	pthread_mutex_lock(&s->state);
	if (!s->dead)
	{
		s->dead = 1;
		ts = now - s->start_ms;
		printf("%ld %d died\n", ts, id);
		fflush(stdout);
	}
	pthread_mutex_unlock(&s->state);
	pthread_mutex_unlock(&s->print);
}

/* --- watchdog thread --- */
void	*watchdog_main(void *arg)
{
	t_shared	*s;
	t_params	*p;
	long		now;
	long		last;
	int			i;

	s = (t_shared *)arg;
	p = s->philos[0].p;
	while (1)
	{
		now = now_ms();
		i = 0;
		while (i < p->n)
		{
			pthread_mutex_lock(&s->state);
			if (s->dead)
				return (pthread_mutex_unlock(&s->state), NULL);
			last = s->philos[i].last_meal_ms;
			pthread_mutex_unlock(&s->state);
			if (now - last > p->time_to_die)
				return (announce_death(s, s->philos[i].id, now), NULL);
			i++;
		}
		sleep_1ms();
	}
}
