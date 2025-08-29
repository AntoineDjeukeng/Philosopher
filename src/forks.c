/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   forks.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 00:00:00 by yourlogin         #+#    #+#             */
/*   Updated: 2025/08/29 11:15:53 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#define _POSIX_C_SOURCE 200809L
#include <time.h>

/* 1) wait until first fork is locked, or abort if someone died */
static int	wait_first(t_shared *s, int f1)
{
	while (1)
	{
		pthread_mutex_lock(&s->state);
		if (s->dead)
			return (pthread_mutex_unlock(&s->state), 0);
		pthread_mutex_unlock(&s->state);
		if (pthread_mutex_trylock(&s->forks[f1]) == 0)
			return (1);
		{ struct timespec ts = (struct timespec){0, 100000}; nanosleep(&ts, NULL); }
	}
}

/* 2) try to lock second fork until deadline; 1 if locked, 0 if timeout/dead */
static int	take_second(t_philo *ph, int f1, int f2)
{
	t_shared	*s;
	long		deadline;

	s = ph->s;
	deadline = now_ms() + ph->p->second_timeout_ms;
	while (now_ms() < deadline)
	{
		pthread_mutex_lock(&s->state);
		if (s->dead)
			return (pthread_mutex_unlock(&s->state), 0);
		pthread_mutex_unlock(&s->state);
		if (pthread_mutex_trylock(&s->forks[f2]) == 0)
			return (1);
		{ struct timespec ts = (struct timespec){0, 100000}; nanosleep(&ts, NULL); }
	}
	return (0);
}

/* 3) ordered acquire; log only after both forks are secured */
int	try_take_both(t_philo *ph)
{
	t_shared	*s;
	int			f1;
	int			f2;

	s = ph->s;
	f1 = (ph->left < ph->right) ? ph->left : ph->right;
	f2 = (ph->left < ph->right) ? ph->right : ph->left;
	if (!wait_first(s, f1))
		return (0);
	if (take_second(ph, f1, f2))
	{
		log_state(ph, "has taken a fork");
		log_state(ph, "has taken a fork");
		return (1);
	}
	pthread_mutex_unlock(&s->forks[f1]);
	return (0);
}

/* 4) release both forks */
void	put_both(t_philo *ph)
{
	pthread_mutex_unlock(&ph->s->forks[ph->left]);
	pthread_mutex_unlock(&ph->s->forks[ph->right]);
}
