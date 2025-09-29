/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 14:39:14 by adjeuken          #+#    #+#             */
/*   Updated: 2025/09/13 14:39:15 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void		util_put_ms_id_msg(long long ms, long long id, const char *msg);

static char	*compose_line(const char *a, const char *b, const char *msg)
{
	char	*line;
	char	*dst;
	size_t	la;
	size_t	lb;
	size_t	lm;

	la = (size_t)str_len(a);
	lb = (size_t)str_len(b);
	lm = (size_t)str_len(msg);
	line = (char *)malloc(la + 1 + lb + 1 + lm + 1);
	if (!line)
		return (NULL);
	dst = line;
	ft_memcpy(dst, a, la);
	dst += la;
	*dst++ = ' ';
	ft_memcpy(dst, b, lb);
	dst += lb;
	*dst++ = ' ';
	ft_memcpy(dst, msg, lm);
	dst += lm;
	*dst = '\0';
	return (line);
}

static char	*build_log_line(long long rel_ms, long long id, const char *msg)
{
	char	*a;
	char	*b;
	char	*line;

	a = ll_to_str(rel_ms);
	b = ll_to_str(id);
	if (!a || !b)
	{
		free(a);
		free(b);
		return (NULL);
	}
	line = compose_line(a, b, msg);
	free(a);
	free(b);
	return (line);
}

void	print_msg(t_philo *p, const char *msg)
{
	long long	rel_ms;
	int			stop;
	char		*line;

	pthread_mutex_lock(&p->par->state_mtx);
	stop = p->par->state;
	pthread_mutex_unlock(&p->par->state_mtx);
	if (stop)
		return ;
    rel_ms = (now_us() - p->par->start_at) / 1000LL;
    if (rel_ms < 0)
        rel_ms = 0;
    line = build_log_line(rel_ms, (long long)p->id, msg);
	if (line)
		logger_enqueue(p->par->logger_ptr, line);
}

int	declare_death(t_philo *p, long long now)
{
	long long	rel_ms;
	t_logger	*lg;

	pthread_mutex_lock(&p->par->state_mtx);
	if (p->par->state != 0)
	{
		pthread_mutex_unlock(&p->par->state_mtx);
		return (0);
	}
	p->par->state = p->id;
	pthread_mutex_unlock(&p->par->state_mtx);
    rel_ms = (now - p->par->start_at) / 1000LL;
    if (rel_ms < 0)
        rel_ms = 0;
    if (p->par->logger_ptr)
	{
		lg = p->par->logger_ptr;
		pthread_mutex_lock(&lg->mtx);
		lg->stop = 1;
		pthread_mutex_unlock(&lg->mtx);
	}
	pthread_mutex_lock(&p->par->print_mtx);
	util_put_ms_id_msg(rel_ms, (long long)p->id, "died\n");
	pthread_mutex_unlock(&p->par->print_mtx);
	return (0);
}
