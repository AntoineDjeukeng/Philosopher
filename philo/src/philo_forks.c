/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_forks.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 14:38:17 by adjeuken          #+#    #+#             */
/*   Updated: 2025/09/13 14:38:18 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	return_forks(t_philo *p)
{
	int	n;

	n = p->par->n;
	pthread_mutex_lock(&p->par->forks[p->left].mtx);
	p->par->forks[p->left].owner = p->left;
	pthread_mutex_unlock(&p->par->forks[p->left].mtx);
	pthread_mutex_lock(&p->par->forks[p->right].mtx);
	p->par->forks[p->right].owner = (p->id + 1) % n;
	pthread_mutex_unlock(&p->par->forks[p->right].mtx);
	p->has_left = 0;
	p->has_right = 0;
}

void	probe_one(t_philo *p, int fork_idx, int is_left)
{
	t_fork	*f;

	f = &p->par->forks[fork_idx];
	pthread_mutex_lock(&f->mtx);
	if (f->owner == p->id)
	{
		if (is_left)
			p->has_left = 1;
		else
			p->has_right = 1;
	}
	pthread_mutex_unlock(&f->mtx);
}

int	take_forks(t_philo *p)
{
	long long	now;

	while (1)
	{
		if (p->has_left && p->has_right)
			return (1);
		if (global_stop(p->par))
			return (0);
		now = now_us();
		if (now - p->last_meal >= p->par->t_die)
			return (declare_death(p, now));
		if (!p->has_left)
			probe_one(p, p->left, 1);
		if (!p->has_right)
			probe_one(p, p->right, 0);
		if (p->has_left && p->has_right)
			return (1);
		micro_sleep(2000);
	}
}
