/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start_threads.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 14:59:17 by adjeuken          #+#    #+#             */
/*   Updated: 2025/09/13 14:59:18 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	start_threads(t_params *par, pthread_t *th, t_philo **ph)
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
