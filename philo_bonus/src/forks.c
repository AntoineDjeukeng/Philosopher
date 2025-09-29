/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   forks.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 14:28:00 by adjeuken          #+#    #+#             */
/*   Updated: 2025/09/13 14:28:01 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	init_forks(t_params *p)
{
	int	i;
	int	owner;
	int	success;

	if (!p || !p->forks)
		return ;
	i = 0;
	success = 1;
	while (i < p->n)
	{
		owner = (i + 1) % p->n;
		if (i % 2 == 0)
			owner = i;
		p->forks[i].left_fork = owner;
		p->forks[i].right_fork = owner;
		if (sem_init(&p->forks[i].lock, 0, 1) != 0)
		{
			success = 0;
			break ;
		}
		i++;
	}
	if (!success)
	{
		while (i-- > 0)
			sem_destroy(&p->forks[i].lock);
		free(p->forks);
		p->forks = NULL;
	}
}

void	destroy_forks(t_params *p)
{
	int	i;

	if (!p || !p->forks)
		return ;
	i = 0;
	while (i < p->n)
		sem_destroy(&p->forks[i++].lock);
	free(p->forks);
	p->forks = NULL;
}
