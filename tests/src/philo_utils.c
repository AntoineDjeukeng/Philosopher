/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 16:07:23 by adjeuken          #+#    #+#             */
/*   Updated: 2025/08/30 16:58:01 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	safe_log(t_philo *p, const char *msg)
{
	int			snap;
	t_logger	*lg;

	snap = 0;
	pthread_mutex_lock(&p->state->state_mx);
	snap = p->state->stop;
	pthread_mutex_unlock(&p->state->state_mx);
	if (snap)
		return ;
	lg = p->state->logger;
	if (lg)
		log_post(lg, p->id, msg);
}

void	ph_think_guard(t_philo *p)
{
	int		odd;
	int		delay;
	int64_t	now;
	int64_t	last;
	int64_t	rem;

	odd = (p->state->rules->n % 2);
	safe_log(p, "is thinking");
	if (!odd)
		return ;
	pthread_mutex_lock(&p->state->state_mx);
	last = p->last_meal_ms;
	pthread_mutex_unlock(&p->state->state_mx);
	now = now_ms();
	rem = p->state->rules->t_die - (now - last);
	delay = p->state->rules->t_eat / 2;
	if ((int64_t)delay > rem - 2)
	{
		if (rem > 2)
			delay = (int)(rem - 2);
		else
			delay = 0;
	}
	if (delay > 0)
		precise_sleep_ms(&p->state->stop, &p->state->state_mx, delay);
}

void	*philo_run(void *arg)
{
	t_philo	*p;

	p = (t_philo *)arg;
	if (p->state->rules->n == 1)
		return (ph_single_case(p), (void *)0);
	ph_desync(p);
	while (!stop_snapshot(p->state))
	{
		ph_think_guard(p);
		take_forks_blocking(p);
		if (stop_snapshot(p->state))
		{
			pthread_mutex_unlock(&p->state->rules->forks[p->left]);
			pthread_mutex_unlock(&p->state->rules->forks[p->right]);
			break ;
		}
		if (ph_eat_block(p))
			break ;
		safe_log(p, "is sleeping");
		precise_sleep_ms(&p->state->stop, &p->state->state_mx,
			p->state->rules->t_sleep);
	}
	return (NULL);
}
