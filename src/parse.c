#include "philo.h"
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

static int	parse_pos_long_arg(const char *s, long *out)
{
	long	val;

	if (!ft_atoi_long(s, &val))
		return (0);
	if (val < 0)
		return (0);
	*out = val;
	return (1);
}

/* expect int in [0, INT_MAX] */
static int	parse_int_arg(const char *s, int *out)
{
	long	val;

	if (!ft_atoi_long(s, &val))
		return (0);
	if (val < 0 || val > INT_MAX)
		return (0);
	*out = (int)val;
	return (1);
}

int	parse_args(int argc, char **argv, t_params *p)
{
	if (!p)
		return (0);
	if (argc != 5 && argc != 6)
		return (0);
	if (!parse_int_arg(argv[1], &p->n))
		return (0);
	if (!parse_pos_long_arg(argv[2], &p->time_to_die))
		return (0);
	if (!parse_pos_long_arg(argv[3], &p->time_to_eat))
		return (0);
	if (!parse_pos_long_arg(argv[4], &p->time_to_sleep))
		return (0);
	if (argc == 6)
	{
		if (!parse_int_arg(argv[5], &p->must_eat))
			return (0);
	}
	else
		p->must_eat = -1;
	p->second_timeout_ms = 5;
	p->retry_backoff_ms = 3;
	return (1);
}



