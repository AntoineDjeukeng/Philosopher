#include "philo.h"
#include <limits.h>

static int	parse_prefix(const char *s, int *i, int *sign)
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

int	ft_atoi_long(const char *str, long *out)
{
	int		i;
	int		sign;
	long	res;

	if (!parse_prefix(str, &i, &sign))
		return (0);
	res = 0;
	while (str[i] >= '0' && str[i] <= '9')
	{
		res = res * 10 + (str[i] - '0');
		i++;
	}
	if (str[i] != '\0')
		return (0);
	if (sign == 1)
		*out = res;
	else
		*out = -res;
	return (1);
}
