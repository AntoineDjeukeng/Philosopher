/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken <adjeuken@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 14:39:14 by adjeuken          #+#    #+#             */
/*   Updated: 2025/09/29 19:25:06 by adjeuken         ###   ########.fr       */
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
	int			stop;
	char		*line;
	long long	rel_ms;

	if (!p->par->state_sem || p->par->state_sem == SEM_FAILED)
		return ;
	sem_wait(p->par->state_sem);
	stop = p->par->state;
	sem_post(p->par->state_sem);
	if (stop)
		return ;
	rel_ms = (now_us() - p->par->start_at) / 1000LL;
	if (rel_ms < 0)
		rel_ms = 0;
	line = build_log_line(rel_ms, (long long)p->id, msg);
	if (line)
		logger_enqueue(p->par->logger_ptr, line);
}

static int	mark_death_and_stop_logger(t_philo *p)
{
	t_logger	*lg;

	if (!p->par->state_sem || p->par->state_sem == SEM_FAILED)
		return (0);
	sem_wait(p->par->state_sem);
	if (p->par->state != 0)
	{
		sem_post(p->par->state_sem);
		return (0);
	}
	p->par->state = p->id;
	sem_post(p->par->state_sem);
	lg = p->par->logger_ptr;
	if (lg && lg->sem && lg->sem != SEM_FAILED)
	{
		sem_wait(lg->sem);
		lg->stop = 1;
		sem_post(lg->sem);
	}
	return (1);
}

int	declare_death(t_philo *p, long long now)
{
	long long	rel_ms;

	if (!mark_death_and_stop_logger(p))
		return (0);
	rel_ms = (now - p->par->start_at) / 1000LL;
	if (rel_ms < 0)
		rel_ms = 0;
	if (!p->par->print_sem || p->par->print_sem == SEM_FAILED)
		return (0);
	sem_wait(p->par->print_sem);
	util_put_ms_id_msg(rel_ms, (long long)p->id, "died\n");
	sem_post(p->par->print_sem);
	return (0);
}
