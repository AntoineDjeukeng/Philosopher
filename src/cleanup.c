#include "philo.h"
#include <stdlib.h>

void	cleanup(t_params *p, t_shared *s)
{
	(void)p;
	if (!s)
		return;
	if (s->forks)
	{
		for (int i = 0; i < s->philos[0].p->n; ++i)
			pthread_mutex_destroy(&s->forks[i]);
		free(s->forks);
		s->forks = NULL;
	}
	pthread_mutex_destroy(&s->print);
	pthread_mutex_destroy(&s->state);
	free(s->philos);
	s->philos = NULL;
}
