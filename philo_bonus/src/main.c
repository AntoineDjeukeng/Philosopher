/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 14:30:48 by adjeuken          #+#    #+#             */
/*   Updated: 2025/09/29 22:14:02 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	handle_single(t_params *par)
{
	char	*str;

	write(1, "0 0 has taken a fork\n", 21);
	usleep((useconds_t)par->t_die);
	str = ll_to_str(par->t_die / 1000LL);
	if (str)
	{
		write(1, str, str_len(str));
		free(str);
	}
	write(1, " 0 died\n", 8);
	destroy_params(par);
	return (0);
}

int	alloc_arrays(pthread_t **th, t_philo ***ph, int n)
{
	int	i;

	*th = (pthread_t *)malloc(sizeof(**th) * n);
	*ph = (t_philo **)malloc(sizeof(**ph) * n);
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

void	cleanup(t_params *par, pthread_t *th, t_philo **ph, int started)
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

static int	run_program(t_params *par)
{
	t_philo		**ph;
	pthread_t	*th;
	int			started;

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

int	main(int ac, char **av)
{
	t_params	*par;

	par = init_params(ac, av);
	if (!par)
		return (1);
	return (run_program(par));
}
