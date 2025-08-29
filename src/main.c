#include "philo.h"
#include <stdio.h>

int	main(int argc, char **argv)
{
	t_params	params;

	if (!parse_args(argc, argv, &params))
	{
		fprintf(stderr, "Usage: %s n t_die t_eat t_sleep [must_eat]\n", argv[0]);
		return (1);
	}
	if (!start_sim(&params))
		return (1);
	return (0);
}
