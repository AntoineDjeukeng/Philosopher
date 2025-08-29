/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 10:56:33 by adjeuken          #+#    #+#             */
/*   Updated: 2025/08/29 11:00:19 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/* check global death and (optionally) this philo's must_eat */
static int	should_stop(t_philo *ph)
{
	int	stop;

	pthread_mutex_lock(&ph->s->state);
	stop = ph->s->dead;
	if (!stop && ph->p->must_eat > 0 && ph->meals_eaten >= ph->p->must_eat)
		stop = 1;
	pthread_mutex_unlock(&ph->s->state);
	return (stop);
}

/* block until forks acquired or stop; then do the eating section */
static void	do_eat(t_philo *ph)
{
	int	got;

	got = 0;
	while (!should_stop(ph))
	{
		if (try_take_both(ph))
		{
			got = 1;
			break ;
		}
		precise_msleep(ph->p->retry_backoff_ms);
	}
	if (!got || should_stop(ph))
		return ;
	pthread_mutex_lock(&ph->s->state);
	ph->last_meal_ms = now_ms();
	pthread_mutex_unlock(&ph->s->state);
	log_state(ph, "is eating");
	precise_msleep(ph->p->time_to_eat);
	pthread_mutex_lock(&ph->s->state);
	ph->meals_eaten++;
	pthread_mutex_unlock(&ph->s->state);
	put_both(ph);
}

void	*philo_main(void *arg)
{
	t_philo	*ph;

	ph = (t_philo *)arg;
	if (ph->id % 2 == 0)
		precise_msleep(ph->p->time_to_eat / 2);
	while (!should_stop(ph))
	{
		log_state(ph, "is thinking");
		do_eat(ph);
		if (should_stop(ph))
			break ;
		log_state(ph, "is sleeping");
		precise_msleep(ph->p->time_to_sleep);
	}
	return (NULL);
}
