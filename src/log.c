#include "philo.h"
#include <stdio.h>

void	log_state(t_philo *ph, const char *msg)
{
	t_shared	*s;
	int			dead;
	long		ts;

	if (!ph || !msg)
		return ;
	s = ph->s;
	pthread_mutex_lock(&s->print);
	pthread_mutex_lock(&s->state);
	dead = s->dead;
	pthread_mutex_unlock(&s->state);
	if (!dead)
	{
		ts = now_ms() - s->start_ms;
		printf("%ld %d %s\n", ts, ph->id, msg);
		fflush(stdout);
	}
	pthread_mutex_unlock(&s->print);
}
