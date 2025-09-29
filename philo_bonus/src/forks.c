/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   forks.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 14:28:00 by adjeuken          #+#    #+#             */
/*   Updated: 2025/09/29 22:43:48 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <fcntl.h>

static void	assign_owners(t_params *p)
{
	int	i;
	int	owner;

	i = 0;
	while (i < p->n)
	{
		if (i % 2 == 0)
			owner = i;
		else
			owner = (i + 1) % p->n;
		p->forks[i].left_fork = owner;
		p->forks[i].right_fork = owner;
		i++;
	}
}

static void	ft_fail(t_params *p, int count)
{
	while (count > 0)
	{
		count--;
		if (p->forks[count].lock && p->forks[count].lock != SEM_FAILED)
			sem_close(p->forks[count].lock);
		p->forks[count].lock = NULL;
	}
	free(p->forks);
	p->forks = NULL;
}

void	init_forks(t_params *p)
{
	int		i;
	char	name[32];

	if (!p || !p->forks || p->n <= 0)
		return ;
	assign_owners(p);
	i = 0;
	while (i < p->n)
	{
		sem_name_from_ptr(name, 'f', (uintptr_t)p, i);
		sem_unlink(name);
		p->forks[i].lock = sem_open(name, O_CREAT | O_EXCL, 0600, 1);
		if (p->forks[i].lock == SEM_FAILED)
		{
			ft_fail(p, i);
			return ;
		}
		sem_unlink(name);
		i++;
	}
}

void	destroy_forks(t_params *p)
{
	int	i;

	if (!p || !p->forks)
		return ;
	i = 0;
	while (i < p->n)
	{
		if (p->forks[i].lock && p->forks[i].lock != SEM_FAILED)
			sem_close(p->forks[i].lock);
		p->forks[i].lock = NULL;
		i++;
	}
	free(p->forks);
	p->forks = NULL;
}
