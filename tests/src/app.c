/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   app.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 16:46:16 by adjeuken          #+#    #+#             */
/*   Updated: 2025/08/30 17:33:54 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	app_run_and_close(t_ctx *a)
{
	int	fail;

	fail = run_threads(a);
	free(a->threads);
	free(a->philos);
	destroy_all(&a->rules, &a->state, &a->logger);
	if (fail)
		return (1);
	return (0);
}

int	app_init(int ac, char **av, t_ctx *a)
{
	if (!init_args(ac, av, &a->rules))
		return (1);
	if (!init_forks(&a->rules))
		return (1);
	logger_init(&a->logger);
	if (!init_state(&a->state, &a->rules, &a->logger))
		return (1);
	a->philos = (t_philo *)ft_calloc((size_t)a->rules.n, sizeof(*a->philos));
	a->threads = (pthread_t *)malloc(sizeof(*a->threads) * (size_t)a->rules.n);
	if (!a->philos || !a->threads)
		return (free(a->threads), free(a->philos), 1);
	init_philos(a->philos, &a->state);
	return (0);
}
