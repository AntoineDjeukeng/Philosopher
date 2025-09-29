/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_lifecycle.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 14:38:45 by adjeuken          #+#    #+#             */
/*   Updated: 2025/09/29 23:32:00 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	philo_sleep_then_think(t_philo *p)
{
	long long	end;
	long long	now;

	end = p->last_meal + p->par->t_eat + p->par->t_sleep;
	print_msg(p, "is sleeping\n");
	while (1)
	{
		now = now_us();
		if (now - p->last_meal >= p->par->t_die)
			return (declare_death(p, now));
		if (!p->has_left)
			probe_one(p, p->left, 1);
		if (!p->has_right)
			probe_one(p, p->right, 0);
		if (p->has_left && p->has_right)
			break ;
		if (now >= end)
			break ;
		micro_sleep(end - now);
	}
	print_msg(p, "is thinking\n");
	return (1);
}

// int	align_start(t_philo *p)
// {
// 	long long	base;
// 	long long	off;
// 	long long	max_off;
// 	long long	deadline;
// 	long long	now;
// 	long long	rem;

// 	base = p->par->start_at;
// 	off = 0;
// 	if ((p->id % 2) != 0)
// 	{
// 		if ((p->par->n % 2) == 0)
// 			off = p->par->t_eat / 2;
// 		else
// 			off = p->par->t_eat / 3;
// 	}
// 	max_off = p->par->t_die - p->par->t_eat - 1000;
// 	if (max_off < 0)
// 		max_off = 0;
// 	if (off > max_off)
// 		off = max_off;
// 	deadline = base + off;
// 	while (1)
// 	{
// 		now = now_us();
// 		if (now >= deadline)
// 			break ;
// 		rem = deadline - now;
// 		if (rem > 500)
// 			micro_sleep(500);
// 		else
// 			micro_sleep(rem);
// 	}
// 	p->last_meal = now_us();
// 	return (1);
// }

int	align_start(t_philo *p)
{
	long long	base;
	long long	off;
	long long	deadline;
	long long	now;

	base = p->par->start_at;
	off = 0;
	if ((p->id % 2) != 0)
	{
		if ((p->par->n % 2) == 0)
			off = p->par->t_eat / 2;
		else
			off = p->par->t_eat / 3;
	}
	deadline = base + off;
	while (1)
	{
		now = now_us();
		if (now >= deadline)
			break ;
		micro_sleep(deadline - now);
	}
	p->last_meal = deadline;
	return (1);
}

int	do_eat(t_philo *p)
{
	p->last_meal = now_us();
	print_msg(p, "has taken a fork\n");
	print_msg(p, "has taken a fork\n");
	print_msg(p, "is eating\n");
	if (p->par->live == 1)
	{
		return_forks(p);
		if (!wait_until_us_or_die(p, p->last_meal + p->par->t_eat))
			return (0);
	}
	else
	{
		if (!wait_until_us_or_die(p, p->last_meal + p->par->t_eat))
			return (0);
		return_forks(p);
	}
	return (1);
}

void	*philosopher_routine(void *arg)
{
	t_philo	*p;

	p = (t_philo *)arg;
	if (!p)
		return (NULL);
	align_start(p);
	while (1)
	{
		if (p->meals == 0 || global_stop(p->par) || !take_forks(p))
			break ;
		if (!do_eat(p))
			return (NULL);
		if (!philo_sleep_then_think(p))
			break ;
		if (p->meals > 0)
			p->meals--;
	}
	return (NULL);
}
