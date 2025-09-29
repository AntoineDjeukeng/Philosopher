/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   num.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 14:40:13 by adjeuken          #+#    #+#             */
/*   Updated: 2025/09/13 14:42:22 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	count_digits_ull(unsigned long long x)
{
	int	len;

	len = 1;
	while (x >= 10ULL)
	{
		x /= 10ULL;
		len++;
	}
	return (len);
}

char	*ll_to_str(long long n)
{
	unsigned long long	x;
	int					neg;
	int					len;
	char				*s;

	neg = (n < 0);
	if (neg)
		x = (unsigned long long)(-(n + 1)) + 1ULL;
	else
		x = (unsigned long long)n;
	len = count_digits_ull(x) + neg;
	s = (char *)malloc((size_t)len + 1);
	if (!s)
		return (NULL);
	s[len] = '\0';
	while (len > neg)
	{
		s[--len] = (char)('0' + (x % 10ULL));
		x /= 10ULL;
	}
	if (neg)
		s[0] = '-';
	return (s);
}

static int	parse_sign(const char *s, int *i)
{
	int	sign;

	sign = 1;
	while (s[*i] == ' ' || (s[*i] >= 9 && s[*i] <= 13))
		(*i)++;
	if (s[*i] == '+' || s[*i] == '-')
	{
		if (s[*i] == '-')
			sign = -1;
		(*i)++;
	}
	return (sign);
}

static void	set_cutoffs(int sign, unsigned long long *cut,
		unsigned long long *cutdig)
{
	if (sign > 0)
	{
		*cut = (unsigned long long)LLONG_MAX / 10ULL;
		*cutdig = (unsigned long long)LLONG_MAX % 10ULL;
	}
	else
	{
		*cut = ((unsigned long long)LLONG_MAX + 1ULL) / 10ULL;
		*cutdig = ((unsigned long long)LLONG_MAX + 1ULL) % 10ULL;
	}
}

long long	parse_ll(const char *s)
{
	int					i;
	int					sign;
	unsigned long long	acc;
	unsigned long long	cutdig;
	unsigned long long	cut;

	i = 0;
	acc = 0;
	sign = parse_sign(s, &i);
	set_cutoffs(sign, &cut, &cutdig);
	while (s[i] >= '0' && s[i] <= '9')
	{
		if (acc > cut || (acc == cut && (unsigned long long)(s[i]
				- '0') > cutdig))
		{
			if (sign > 0)
				return (LLONG_MAX);
			return (LLONG_MIN);
		}
		acc = acc * 10ULL + (unsigned long long)(s[i] - '0');
		i++;
	}
	if (sign > 0)
		return ((long long)acc);
	return (-(long long)acc);
}
