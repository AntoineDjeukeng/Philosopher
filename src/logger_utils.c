/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 16:42:11 by adjeuken          #+#    #+#             */
/*   Updated: 2025/08/30 18:43:47 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	logger_clear_and_set_death(t_logger *lg, int victim_id,
		int64_t rel_ms)
{
	t_logmsg	*t;
	t_logmsg	*m;

	while (lg->head)
	{
		t = lg->head;
		lg->head = t->next;
		free(t);
	}
	lg->tail = NULL;
	m = (t_logmsg *)malloc(sizeof(*m));
	if (m)
	{
		m->rel_ms = rel_ms;
		m->id = victim_id;
		ft_strlcpy(m->text, "died", sizeof(m->text));
		m->next = NULL;
		lg->head = m;
		lg->tail = m;
	}
}

t_logmsg	*logger_pop(t_logger *lg, int *killed, int *stop)
{
	t_logmsg	*m;

	pthread_mutex_lock(&lg->mx);
	if (!lg->head)
	{
		*stop = lg->stop;
		pthread_mutex_unlock(&lg->mx);
		return (NULL);
	}
	m = lg->head;
	lg->head = m->next;
	if (!lg->head)
		lg->tail = NULL;
	*killed = lg->killed;
	*stop = lg->stop;
	pthread_mutex_unlock(&lg->mx);
	return (m);
}

t_logmsg	*logmsg_make(int64_t rel, int id, const char *msg)
{
	t_logmsg	*m;
	size_t		n;

	m = (t_logmsg *)malloc(sizeof(*m));
	if (!m)
		return (NULL);
	m->rel_ms = rel;
	m->id = id;
	n = sizeof(m->text) - 1;
	ft_strlcpy(m->text, msg, n);
	m->text[n] = '\0';
	m->next = NULL;
	return (m);
}

int	ft_puterr(const char *s)
{
	int	i;

	i = 0;
	while (s[i])
		i++;
	if (write(2, s, i) < 0)
		return (-1);
	return (0);
}
