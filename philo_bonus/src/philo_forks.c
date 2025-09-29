/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_forks.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken <adjeuken@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 14:38:17 by adjeuken          #+#    #+#             */
/*   Updated: 2025/09/29 16:29:32 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	return_forks(t_philo *p)
{
	int		n;
	int		right_neighbor;
	t_fork	*left;
	t_fork	*right;

	n = p->par->n;
	right_neighbor = (p->id + 1) % n;
	left = &p->par->forks[p->left];
	right = &p->par->forks[p->right];
	sem_wait(left->lock);
	left->left_fork = p->left;
	left->right_fork = p->left;
	sem_post(left->lock);
	sem_wait(right->lock);
	right->left_fork = right_neighbor;
	right->right_fork = right_neighbor;
	sem_post(right->lock);
	p->has_left = 0;
	p->has_right = 0;
}

void	probe_one(t_philo *p, int fork_idx, int is_left)
{
	t_fork	*f;

	f = &p->par->forks[fork_idx];
	sem_wait(f->lock);
	if (is_left)
	{
		if (f->right_fork == p->id)
			p->has_left = 1;
	}
	else if (f->left_fork == p->id)
		p->has_right = 1;
	sem_post(f->lock);
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
