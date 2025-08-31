/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 16:42:11 by adjeuken          #+#    #+#             */
/*   Updated: 2025/08/30 18:32:51 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	logger_kill_with_death(t_logger *lg, int victim_id, int64_t rel_ms)
{
	pthread_mutex_lock(&lg->mx);
	if (!lg->killed)
	{
		lg->killed = 1;
		logger_clear_and_set_death(lg, victim_id, rel_ms);
	}
	lg->stop = 1;
	pthread_mutex_unlock(&lg->mx);
}

void	log_post(t_logger *lg, int id, const char *msg)
{
	t_logmsg	*m;
	int64_t		rel;

	pthread_mutex_lock(&lg->mx);
	if (lg->killed)
	{
		pthread_mutex_unlock(&lg->mx);
		return ;
	}
	rel = now_ms() - lg->start_ms;
	m = logmsg_make(rel, id, msg);
	if (!m)
	{
		pthread_mutex_unlock(&lg->mx);
		return ;
	}
	if (lg->tail)
		lg->tail->next = m;
	else
		lg->head = m;
	lg->tail = m;
	pthread_mutex_unlock(&lg->mx);
}

void	*logger_thread(void *arg)
{
	int			killed;
	int			stop;
	t_logger	*lg;
	t_logmsg	*m;

	lg = (t_logger *)arg;
	while (1)
	{
		m = logger_pop(lg, &killed, &stop);
		if (!m)
		{
			if (stop)
				break ;
			usleep(1000);
			continue ;
		}
		if (killed && ft_strcmp(m->text, "died") != 0)
			free(m);
		else
		{
			printf("%lld %d %s\n", (long long)m->rel_ms, m->id, m->text);
			free(m);
		}
	}
	return (NULL);
}
