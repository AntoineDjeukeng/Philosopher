/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   new.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 22:36:49 by adjeuken          #+#    #+#             */
/*   Updated: 2025/09/11 01:04:14 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "new.h"

int	global_stop(t_params *par)
{
	int	stop;

	pthread_mutex_lock(&par->g_state_locker);
	stop = (par->g_state != 0);
	pthread_mutex_unlock(&par->g_state_locker);
	return (stop);
}

void	micro_pause(long long rem)
{
	if (rem <= 150)
		return ;
	if (rem > 300)
		rem = 300;
	else if (rem < 0)
		rem = 0;
	usleep((useconds_t)rem);
}

int	wait_until_us_or_die(t_philosopher *p, long long tgt)
{
	long long	now;

	while (1)
	{
		now = now_us();
		if (now - p->last_meal >= p->params->to_die)
			return (declare_death(p, now));
		if (now >= tgt)
			return (1);
		micro_pause(tgt - now);
	}
}

void	probe_one(t_philosopher *p, int fork_idx, int is_left)
{
	t_forks	*f;

	f = &p->params->forks[fork_idx];
	pthread_mutex_lock(&f->locker);
	if (f->id == p->me)
	{
		if (is_left)
			p->gotl = 1;
		else
			p->gotr = 1;
	}
	pthread_mutex_unlock(&f->locker);
}

int	take_forks(t_philosopher *p)
{
	long long	now;

	while (1)
	{
		if (p->gotl && p->gotr)
			return (1);
		if (global_stop(p->params))
			return (0);
		now = now_us();
		if (now - p->last_meal >= p->params->to_die)
			return (declare_death(p, now));
		if (!p->gotl)
			probe_one(p, p->left, 1);
		if (!p->gotr)
			probe_one(p, p->right, 0);
		if (p->gotl && p->gotr)
			return (1);
		micro_pause(2000);
	}
}

int	philo_sleep_then_think(t_philosopher *p)
{
	long long	end;
	long long	now;

	end = p->last_meal + p->params->to_eat + p->params->to_sleep;
	print_msg(p, "is sleeping\n");
	while (1)
	{
		now = now_us();
		if (now - p->last_meal >= p->params->to_die)
			return (declare_death(p, now));
		if (!p->gotl)
			probe_one(p, p->left, 1);
		if (!p->gotr)
			probe_one(p, p->right, 0);
		if (p->gotl && p->gotr)
			break ;
		if (now >= end)
			break ;
		micro_pause(end - now);
	}
	print_msg(p, "is thinking\n");
	return (1);
}

/* ================= start alignment ================= */

int	align_start(t_philosopher *p)
{
	long long	base;
	long long	off;
	long long	deadline;
	long long	now;

	base = p->params->start_at;
	off = 0;
	if ((p->me % 2) != 0)
	{
		if ((p->params->n % 2) == 0)
			off = p->params->to_eat / 2;
		else
			off = p->params->to_eat / 3;
	}
	deadline = base + off;
	while (1)
	{
		now = now_us();
		if (now >= deadline)
			break ;
		micro_pause(deadline - now);
	}
	p->last_meal = deadline;
	return (1);
}

int	do_eat(t_philosopher *p)
{
	p->last_meal = now_us();
	print_msg(p, "has taken a fork\n");
	print_msg(p, "has taken a fork\n");
	print_msg(p, "is eating\n");
	if (p->params->live == 1)
	{
		return_forks(p);
		if (!wait_until_us_or_die(p, p->last_meal + p->params->to_eat))
			return (0);
	}
	else
	{
		if (!wait_until_us_or_die(p, p->last_meal + p->params->to_eat))
			return (0);
		return_forks(p);
	}
	return (1);
}

void	*philosopher_routine(void *arg)
{
	t_philosopher	*p;

	p = (t_philosopher *)arg;
	if (!p)
		return (NULL);
	align_start(p);
	while (1)
	{
		if (p->meals == 0 || global_stop(p->params) || !take_forks(p))
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

int	handle_single(t_params *par)
{
	long long	ms0;
	char		*str;

	ms0 = now_us() / 1000LL;
	write(1, "0 0 has taken a fork\n", 21);
	usleep((useconds_t)par->to_die);
	str = ft_itoll(ms0 + par->to_die / 1000LL);
	if (str)
		ft_putstr_s(str, 1);
	write(1, " 0 died\n", 8);
	destroy_params(par);
	return (0);
}

int	alloc_arrays(pthread_t **th, t_philosopher ***ph, int n)
{
	int	i;

	*th = (pthread_t *)malloc(sizeof(**th) * n);
	*ph = (t_philosopher **)malloc(sizeof(**ph) * n);
	if (!*th || !*ph)
	{
		free(*th);
		free(*ph);
		*th = NULL;
		*ph = NULL;
		return (0);
	}
	i = 0;
	while (i < n)
	{
		(*ph)[i] = NULL;
		i++;
	}
	return (1);
}

int	start_threads(t_params *par, pthread_t *th, t_philosopher **ph)
{
	int	i;

	i = 0;
	while (i < par->n)
	{
		ph[i] = init_philosopher(i, par);
		if (!ph[i] || pthread_create(&th[i], NULL, philosopher_routine,
				ph[i]) != 0)
		{
			if (ph[i])
				free(ph[i]);
			break ;
		}
		i++;
	}
	return (i);
}

void	cleanup(t_params *par, pthread_t *th, t_philosopher **ph, int started)
{
	int	i;

	i = 0;
	while (i < started)
	{
		pthread_join(th[i], NULL);
		i++;
	}
	i = 0;
	while (i < started)
	{
		if (ph[i])
			free(ph[i]);
		i++;
	}
	free(th);
	free(ph);
	destroy_params(par);
}

int	main(int ac, char **av)
{
	t_params		*par;
	t_philosopher	**ph;
	pthread_t		*th;
	int				started;

	par = init_params(ac, av);
	if (!par)
		return (1);
	if (par->n == 1)
		return (handle_single(par));
	if (!alloc_arrays(&th, &ph, par->n))
	{
		destroy_params(par);
		return (1);
	}
	started = start_threads(par, th, ph);
	if (started != par->n)
	{
		cleanup(par, th, ph, started);
		return (1);
	}
	cleanup(par, th, ph, started);
	return (0);
}
