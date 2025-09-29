/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   params_core.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 14:36:30 by adjeuken          #+#    #+#             */
/*   Updated: 2025/09/13 14:36:31 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long	ceil_div(long a, long b)
{
	if (b <= 0)
		return (LONG_MAX);
	return ((a + b - 1) / b);
}

int	should_live_ms(t_params *p)
{
	int	e;
	int	d;
	int	base;

	e = (int)(p->t_eat / 1000LL);
	d = (int)(p->t_die / 1000LL);
	base = e + (int)(p->t_sleep / 1000LL);
	if ((p->n % 2) == 0)
	{
		if (base < 2 * e)
			base = 2 * e;
	}
	else
	{
		if (base < 3 * e)
			base = 3 * e;
	}
	return (d > base + 50 / p->n);
}

long	get_cycle(long n, long t_die, long t_eat, long t_sleep)
{
	long	m;
	long	r;
	long	need;
	long	gap;

	if (n <= 0 || t_die < 0 || t_eat < 0 || t_sleep < 0)
		return (-1);
	m = n / 2;
	if (n < 2 || t_eat >= t_die || m == 0)
		return (0);
	r = ceil_div(n, m) * t_eat;
	need = t_eat + t_sleep;
	if (need >= t_die)
		return (0);
	gap = ceil_div(need, r) * r;
	if (gap < t_die)
		return (1);
	return (0);
}
