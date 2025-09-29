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
	int	j;

	if (!p || !p->forks)
		return ;
	i = 0;
	while (i < p->n)
	{
		p->forks[i].owner = (i + 1) % p->n;
		if (i % 2 == 0)
			p->forks[i].owner = i;
		if (pthread_mutex_init(&p->forks[i].mtx, NULL) != 0)
		{
			j = 0;
			while (j < i)
				pthread_mutex_destroy(&p->forks[j++].mtx);
			free(p->forks);
			p->forks = NULL;
			return ;
		}
		i++;
	}
}

void	destroy_forks(t_params *p)
{
	int	i;

	i = 0;
	if (!p || !p->forks)
		return ;
	while (i < p->n)
	{
		pthread_mutex_destroy(&p->forks[i].mtx);
		i++;
	}
	free(p->forks);
	p->forks = NULL;
}
