/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   params_init.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 14:37:14 by adjeuken          #+#    #+#             */
/*   Updated: 2025/09/13 14:37:15 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	init_mutexes_and_forks(t_params *p)
{
	if (p->n < 1 || pthread_mutex_init(&p->state_mtx, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&p->print_mtx, NULL) != 0)
	{
		pthread_mutex_destroy(&p->state_mtx);
		return (0);
	}
	if (p->n > 1)
	{
		p->forks = (t_fork *)malloc(sizeof(t_fork) * p->n);
		if (!p->forks)
		{
			pthread_mutex_destroy(&p->print_mtx);
			pthread_mutex_destroy(&p->state_mtx);
			return (0);
		}
		init_forks(p);
	}
	return (1);
}

static int	init_logger_wrap(t_params *p)
{
	p->logger_ptr = (t_logger *)malloc(sizeof(t_logger));
	if (!p->logger_ptr)
		return (0);
	if (!logger_init(p->logger_ptr, 256))
		return (0);
	if (!logger_start(p->logger_ptr))
		return (0);
	return (1);
}

int	setup_all(t_params *p)
{
	if (!init_mutexes_and_forks(p))
		return (0);
	p->live = get_cycle(p->n, (long)(p->t_die / 1000LL), (long)(p->t_eat
				/ 1000LL), (long)(p->t_sleep / 1000LL));
	p->start_at = now_us() + 2000LL;
	if (!init_logger_wrap(p))
		return (0);
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
	p->state = 0;
	p->forks = NULL;
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
	pthread_mutex_destroy(&p->print_mtx);
	pthread_mutex_destroy(&p->state_mtx);
	free(p);
}
