/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   params_init.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 14:37:14 by adjeuken          #+#    #+#             */
/*   Updated: 2025/09/29 22:35:55 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <fcntl.h>

static int	init_sems_and_forks(t_params *p)
{
	if (p->n < 1)
		return (0);
	if (!open_param_sem(&p->state_sem, p, 's'))
		return (0);
	if (!open_param_sem(&p->print_sem, p, 'p'))
	{
		close_params_sems(p);
		return (0);
	}
	if (p->n > 1)
	{
		p->forks = (t_fork *)malloc(sizeof(t_fork) * p->n);
		if (!p->forks)
		{
			close_params_sems(p);
			return (0);
		}
		init_forks(p);
		if (!p->forks)
		{
			close_params_sems(p);
			return (0);
		}
	}
	return (1);
}

int	init_logger_wrap(t_params *p)
{
	p->logger_ptr = (t_logger *)malloc(sizeof(t_logger));
	if (!p->logger_ptr)
		return (0);
	if (!logger_init(p->logger_ptr, 256))
	{
		free(p->logger_ptr);
		p->logger_ptr = NULL;
		return (0);
	}
	if (!logger_start(p->logger_ptr))
	{
		logger_destroy(p->logger_ptr);
		free(p->logger_ptr);
		p->logger_ptr = NULL;
		return (0);
	}
	return (1);
}

int	setup_all(t_params *p)
{
	p->state = 0;
	p->forks = NULL;
	p->state_sem = NULL;
	p->print_sem = NULL;
	if (!init_sems_and_forks(p))
		return (0);
	p->live = get_cycle(p->n, (long)(p->t_die / 1000LL), (long)(p->t_eat
				/ 1000LL), (long)(p->t_sleep / 1000LL));
	p->start_at = now_us() + 2000LL;
	if (!init_logger_wrap(p))
	{
		if (p->forks)
			destroy_forks(p);
		close_params_sems(p);
		return (0);
	}
	return (1);
}

t_params	*init_params(int argc, char **argv)
{
	t_params	*p;

	if (argc < 5)
	{
		write(2, "invalid argument\n", 17);
		return (NULL);
	}
	p = (t_params *)malloc(sizeof(*p));
	if (!p)
		return (NULL);
	p->n = (int)parse_ll(argv[1]);
	p->t_die = parse_ll(argv[2]) * 1000LL;
	p->t_eat = parse_ll(argv[3]) * 1000LL;
	p->t_sleep = parse_ll(argv[4]) * 1000LL;
	p->must_eat = -1;
	if (argc >= 6)
		p->must_eat = (int)parse_ll(argv[5]);
	if (!setup_all(p))
	{
		free(p);
		return (NULL);
	}
	return (p);
}

void	destroy_params(t_params *p)
{
	if (!p)
		return ;
	if (p->logger_ptr)
	{
		logger_stop_and_join(p->logger_ptr);
		logger_destroy(p->logger_ptr);
		free(p->logger_ptr);
		p->logger_ptr = NULL;
	}
	if (p->forks)
		destroy_forks(p);
	if (p->print_sem && p->print_sem != SEM_FAILED)
		sem_close(p->print_sem);
	p->print_sem = NULL;
	if (p->state_sem && p->state_sem != SEM_FAILED)
		sem_close(p->state_sem);
	p->state_sem = NULL;
	free(p);
}
