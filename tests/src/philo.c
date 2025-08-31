/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 16:07:23 by adjeuken          #+#    #+#             */
/*   Updated: 2025/08/30 17:04:24 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	take_forks_blocking(t_philo *p)
{
	t_rules	*rules;
	int		a;
	int		b;

	rules = p->state->rules;
	a = p->left;
	b = p->right;
	if (p->id % 2 == 0)
	{
		a = p->right;
		b = p->left;
	}
	pthread_mutex_lock(&rules->forks[a]);
	safe_log(p, "has taken a fork");
	pthread_mutex_lock(&rules->forks[b]);
	safe_log(p, "has taken a fork");
}

void	ph_single_case(t_philo *p)
{
	t_rules	*rules;
	int64_t	now;
	int64_t	last;
	int64_t	rem;
	int		delay;

	rules = p->state->rules;
	safe_log(p, "is thinking");
	pthread_mutex_lock(&rules->forks[p->left]);
	safe_log(p, "has taken a fork");
	pthread_mutex_lock(&p->state->state_mx);
	last = p->last_meal_ms;
	pthread_mutex_unlock(&p->state->state_mx);
	now = now_ms();
	rem = rules->t_die - (now - last);
	delay = 0;
	if (rem > 2)
		delay = (int)(rem - 2);
	if (delay > 0 && !stop_snapshot(p->state))
		precise_sleep_ms(&p->state->stop, &p->state->state_mx, delay);
	wait_until_stop(p->state);
	pthread_mutex_unlock(&rules->forks[p->left]);
}

void	ph_desync(t_philo *p)
{
	t_rules	*rules;
	int		d;

	rules = p->state->rules;
	d = 0;
	if (rules->n % 2 == 0 && p->id % 2 == 0)
		d = 2;
	if (rules->n % 2 == 1 && p->id % 2 == 0)
		d = rules->t_eat / 2;
	if (d > 0)
		precise_sleep_ms(&p->state->stop, &p->state->state_mx, d);
}

int	ph_eat_block(t_philo *p)
{
	t_rules	*rules;
	int		done;

	rules = p->state->rules;
	done = 0;
	safe_log(p, "is eating");
	pthread_mutex_lock(&p->state->state_mx);
	p->last_meal_ms = now_ms();
	pthread_mutex_unlock(&p->state->state_mx);
	precise_sleep_ms(&p->state->stop, &p->state->state_mx, rules->t_eat);
	pthread_mutex_unlock(&p->state->rules->forks[p->left]);
	pthread_mutex_unlock(&p->state->rules->forks[p->right]);
	p->meals++;
	if (rules->t_meals > 0 && (int)p->meals >= rules->t_meals)
		done = 1;
	return (done);
}
