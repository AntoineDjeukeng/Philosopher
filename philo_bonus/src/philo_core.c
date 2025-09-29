/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_core.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken <adjeuken@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 14:37:37 by adjeuken          #+#    #+#             */
/*   Updated: 2025/09/29 19:20:43 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	global_stop(t_params *par)
{
	int	stop;

	if (!par->state_sem || par->state_sem == SEM_FAILED)
		return (1);
	sem_wait(par->state_sem);
	stop = (par->state != 0);
	sem_post(par->state_sem);
	return (stop);
}

t_philo	*init_philosopher(int me, t_params *par)
{
	t_philo	*p;

	if (!par || par->n < 2)
		return (NULL);
	p = (t_philo *)malloc(sizeof(*p));
	if (!p)
		return (NULL);
	p->id = me;
	p->left = (me + par->n - 1) % par->n;
	p->right = me;
	p->par = par;
	p->meals = par->must_eat;
	p->last_meal = now_us();
	p->has_left = 0;
	p->has_right = 0;
	return (p);
}

int	wait_until_us_or_die(t_philo *p, long long tgt)
{
	long long	now;

	while (1)
	{
		now = now_us();
		if (now - p->last_meal >= p->par->t_die)
			return (declare_death(p, now));
		if (now >= tgt)
			return (1);
		micro_sleep(tgt - now);
	}
}
