/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 16:40:56 by adjeuken          #+#    #+#             */
/*   Updated: 2025/08/30 18:13:49 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	rulesrse_prefix(const char *s, int *i, int *sign)
{
	int	k;

	if (!s || !*s)
		return (0);
	k = 0;
	while (s[k] == ' ' || (s[k] >= 9 && s[k] <= 13))
		k++;
	*sign = 1;
	if (s[k] == '+' || s[k] == '-')
	{
		if (s[k] == '-')
			*sign = -1;
		k++;
	}
	if (s[k] < '0' || s[k] > '9')
		return (0);
	*i = k;
	return (1);
}

int	ft_atoi_long(const char *str, int min, int max, int *out)
{
	int		i;
	int		sign;
	long	res;

	if (!rulesrse_prefix(str, &i, &sign))
		return (ft_puterr("Invalid args\n"), 0);
	res = 0;
	while (str[i] >= '0' && str[i] <= '9')
	{
		res = res * 10 + (str[i] - '0');
		if (res > (long)max + (sign == -1))
			return (ft_puterr("Invalid args\n"), 0);
		i++;
	}
	if (str[i] != '\0')
		return (ft_puterr("Invalid args\n"), 0);
	if (sign == -1)
		res = -res;
	if (res < (long)min || res > (long)max)
		return (ft_puterr("Invalid args\n"), 0);
	*out = (int)res;
	return (1);
}

int	init_args(int ac, char **av, t_rules *rules)
{
	if (ac < 5 || ac > 6)
	{
		printf("Usage: %s N t_die t_eat t_sleep [t_meals]\n", av[0]);
		return (0);
	}
	if (!ft_atoi_long(av[1], 1, 1000000000, &rules->n))
		return (0);
	if (!ft_atoi_long(av[2], 0, 1000000000, &rules->t_die))
		return (0);
	if (!ft_atoi_long(av[3], 0, 1000000000, &rules->t_eat))
		return (0);
	if (!ft_atoi_long(av[4], 0, 1000000000, &rules->t_sleep))
		return (0);
	rules->t_meals = -1;
	if (ac == 6)
	{
		if (!ft_atoi_long(av[5], 1, 1000000000, &rules->t_meals))
			return (0);
	}
	return (1);
}
