/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 00:00:00 by yourlogin         #+#    #+#             */
/*   Updated: 2025/08/29 10:49:42 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <stdlib.h>

/* 1) init global fields and mutexes */
static int	init_mutexes(const t_params *p, t_shared *s)
{
	(void)p;
	s->forks = NULL;
	s->philos = NULL;
	s->dead = 0;
	s->start_ms = now_ms();
	if (pthread_mutex_init(&s->print, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&s->state, NULL) != 0)
	{
		pthread_mutex_destroy(&s->print);
		return (0);
	}
	return (1);
}

/* 2) alloc/init forks array */
static int	init_forks(const t_params *p, t_shared *s)
{
	int	i;

	s->forks = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t) * p->n);
	if (!s->forks)
		return (0);
	i = 0;
	while (i < p->n)
	{
		if (pthread_mutex_init(&s->forks[i], NULL) != 0)
		{
			while (i > 0)
			{
				i--;
				pthread_mutex_destroy(&s->forks[i]);
			}
			free(s->forks);
			s->forks = NULL;
			return (0);
		}
		i++;
	}
	return (1);
}

/* 3) alloc/init philosophers array */
static int	init_philos(const t_params *p, t_shared *s)
{
	int	i;

	s->philos = (t_philo *)malloc(sizeof(t_philo) * p->n);
	if (!s->philos)
		return (0);
	i = 0;
	while (i < p->n)
	{
		s->philos[i].id = i + 1;
		s->philos[i].left = i;
		s->philos[i].right = (i + 1) % p->n;
		s->philos[i].last_meal_ms = s->start_ms;
		s->philos[i].meals_eaten = 0;
		s->philos[i].p = (t_params *)p;
		s->philos[i].s = s;
		i++;
	}
	return (1);
}

/* 4) destroy/free everything */
static void	free_sim(const t_params *p, t_shared *s)
{
	int	i;

	if (s->philos)
	{
		free(s->philos);
		s->philos = NULL;
	}
	if (s->forks)
	{
		i = 0;
		while (i < p->n)
		{
			pthread_mutex_destroy(&s->forks[i]);
			i++;
		}
		free(s->forks);
		s->forks = NULL;
	}
	pthread_mutex_destroy(&s->state);
	pthread_mutex_destroy(&s->print);
}

/* 5) create philosopher threads + watchdog; set dead on failure */
static void	spawn_threads(const t_params *p, t_shared *s, pthread_t *wd)
{
	int	i;

	i = 0;
	while (i < p->n)
	{
		if (pthread_create(&s->philos[i].thread, NULL,
				philo_main, &s->philos[i]) != 0)
		{
			pthread_mutex_lock(&s->state);
			s->dead = 1;
			pthread_mutex_unlock(&s->state);
			break ;
		}
		i++;
	}
	if (pthread_create(wd, NULL, watchdog_main, s) != 0)
	{
		pthread_mutex_lock(&s->state);
		s->dead = 1;
		pthread_mutex_unlock(&s->state);
	}
}

/* 6) public: create, run, join, cleanup */
int	start_sim(const t_params *p)
{
	t_shared	s;
	pthread_t	wd;
	int			i;

	if (!p || p->n <= 0 || !init_mutexes(p, &s))
		return (0);
	if (!init_forks(p, &s) || !init_philos(p, &s))
	{
		free_sim(p, &s);
		return (0);
	}
	spawn_threads(p, &s, &wd);
	pthread_join(wd, NULL);
	i = 0;
	while (i < p->n)
	{
		pthread_join(s.philos[i].thread, NULL);
		i++;
	}
	free_sim(p, &s);
	return (1);
}
