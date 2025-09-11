/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   new.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 22:58:58 by adjeuken          #+#    #+#             */
/*   Updated: 2025/09/11 01:08:51 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <limits.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

typedef struct s_forks
{
	int				id;
	pthread_mutex_t	locker;
}					t_forks;

long long			now_us(void);

typedef struct s_params
{
	int				n;
	int				g_state;
	long long		to_die;
	long long		to_eat;
	long long		to_sleep;
	int				live;
	int				must_eat;
	long long		start_at;
	pthread_mutex_t	g_state_locker;
	pthread_mutex_t	g_print_locker;
	t_forks			*forks;
}					t_params;

typedef struct s_philosopher
{
	int				me;
	int				left;
	int				right;
	int				meals;
	long long		last_meal;
	int				gotl;
	int				gotr;
	t_params		*params;
}					t_philosopher;

long long	now_us(void)
{
	static struct timeval	epoch;
	struct timeval			cur;
	long long				sec;
	long long				usec;

	gettimeofday(&cur, NULL);
	if (epoch.tv_sec == 0 && epoch.tv_usec == 0)
	{
		epoch.tv_sec = cur.tv_sec;
		epoch.tv_usec = cur.tv_usec;
	}
	sec = (long long)(cur.tv_sec - epoch.tv_sec);
	usec = (long long)(cur.tv_usec - epoch.tv_usec);
	return (sec * 1000000LL + usec);
}

static int	count_digits_ull(unsigned long long x)
{
	int	len;

	len = 1;
	while (x >= 10ULL)
	{
		x /= 10ULL;
		len++;
	}
	return (len);
}

char	*ft_itoll(long long n)
{
	unsigned long long	x;
	int					neg;
	int					len;
	char				*s;

	neg = (n < 0);
	if (neg)
		x = (unsigned long long)(-(n + 1)) + 1ULL;
	else
		x = (unsigned long long)n;
	len = count_digits_ull(x) + neg;
	s = (char *)malloc((size_t)len + 1);
	if (!s)
		return (NULL);
	s[len] = '\0';
	while (len > neg)
	{
		s[--len] = (char)('0' + (x % 10ULL));
		x /= 10ULL;
	}
	if (neg)
		s[0] = '-';
	return (s);
}

int	msg_len(const char *s)
{
	int	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

static int	parse_sign(const char *s, int *i)
{
	int	sign;

	sign = 1;
	while (s[*i] == ' ' || (s[*i] >= 9 && s[*i] <= 13))
		(*i)++;
	if (s[*i] == '+' || s[*i] == '-')
	{
		if (s[*i] == '-')
			sign = -1;
		(*i)++;
	}
	return (sign);
}

static void	set_cutoffs(int sign, unsigned long long *cut,
		unsigned long long *cutdig)
{
	if (sign > 0)
	{
		*cut = (unsigned long long)LLONG_MAX / 10ULL;
		*cutdig = (unsigned long long)LLONG_MAX % 10ULL;
	}
	else
	{
		*cut = ((unsigned long long)LLONG_MAX + 1ULL) / 10ULL;
		*cutdig = ((unsigned long long)LLONG_MAX + 1ULL) % 10ULL;
	}
}

long long	ft_atoill(const char *s)
{
	int					i;
	int					sign;
	unsigned long long	acc;
	unsigned long long	cut;
	unsigned long long	cutdig;

	i = 0;
	acc = 0;
	sign = parse_sign(s, &i);
	set_cutoffs(sign, &cut, &cutdig);
	while (s[i] >= '0' && s[i] <= '9')
	{
		if (acc > cut || (acc == cut && (unsigned long long)(s[i]
				- '0') > cutdig))
		{
			if (sign > 0)
				return (LLONG_MAX);
			return (LLONG_MIN);
		}
		acc = acc * 10ULL + (unsigned long long)(s[i] - '0');
		i++;
	}
	if (sign > 0)
		return ((long long)acc);
	return (-(long long)acc);
}

void	ft_putstr_s(char *s, int do_free)
{
	int	i;

	if (!s)
		return ;
	i = 0;
	while (s[i])
		i++;
	if (i > 0)
		write(1, s, i);
	if (do_free)
		free(s);
}

void	print_msg(t_philosopher *p, const char *msg)
{
	long long	rel_ms;
	int			stop;

	pthread_mutex_lock(&p->params->g_state_locker);
	stop = p->params->g_state;
	pthread_mutex_unlock(&p->params->g_state_locker);
	if (stop)
		return ;
	rel_ms = (now_us() - p->params->start_at) / 1000LL;
	pthread_mutex_lock(&p->params->g_print_locker);
	ft_putstr_s(ft_itoll(rel_ms), 1);
	write(1, " ", 1);
	ft_putstr_s(ft_itoll((long long)p->me), 1);
	write(1, " ", 1);
	write(1, msg, (int)msg_len(msg));
	pthread_mutex_unlock(&p->params->g_print_locker);
}

int	declare_death(t_philosopher *p, long long now)
{
	long long	rel_ms;

	pthread_mutex_lock(&p->params->g_state_locker);
	if (p->params->g_state != 0)
	{
		pthread_mutex_unlock(&p->params->g_state_locker);
		return (0);
	}
	p->params->g_state = p->me;
	pthread_mutex_unlock(&p->params->g_state_locker);
	rel_ms = (now - p->params->start_at) / 1000LL;
	pthread_mutex_lock(&p->params->g_print_locker);
	ft_putstr_s(ft_itoll(rel_ms), 1);
	write(1, " ", 1);
	ft_putstr_s(ft_itoll((long long)p->me), 1);
	write(1, " died\n", 6);
	pthread_mutex_unlock(&p->params->g_print_locker);
	return (0);
}

t_philosopher	*init_philosopher(int me, t_params *par)
{
	t_philosopher	*p;

	if (!par || par->n < 2)
		return (NULL);
	p = (t_philosopher *)malloc(sizeof(*p));
	if (!p)
		return (NULL);
	p->me = me;
	p->left = (me + par->n - 1) % par->n;
	p->right = me;
	p->params = par;
	p->meals = par->must_eat;
	p->last_meal = now_us();
	p->gotl = 0;
	p->gotr = 0;
	return (p);
}

void	return_forks(t_philosopher *p)
{
	int	n;

	n = p->params->n;
	pthread_mutex_lock(&p->params->forks[p->left].locker);
	p->params->forks[p->left].id = p->left;
	pthread_mutex_unlock(&p->params->forks[p->left].locker);
	pthread_mutex_lock(&p->params->forks[p->right].locker);
	p->params->forks[p->right].id = (p->me + 1) % n;
	pthread_mutex_unlock(&p->params->forks[p->right].locker);
	p->gotl = 0;
	p->gotr = 0;
}

void	init_forks(t_params *p)
{
	int	i;
	int	j;

	if (!p || !p->forks)
		return ;
	i = 0;
	while (i < p->n)
	{
		p->forks[i].id = (i + 1) % p->n;
		if (i % 2 == 0)
			p->forks[i].id = i;
		if (pthread_mutex_init(&p->forks[i].locker, NULL) != 0)
		{
			j = 0;
			while (j < i)
				pthread_mutex_destroy(&p->forks[j++].locker);
			free(p->forks);
			p->forks = NULL;
			return ;
		}
		i++;
	}
}

void	destroy_forks(t_params *p)
{
	int	i;

	if (!p || !p->forks)
		return ;
	i = 0;
	while (i < p->n)
	{
		pthread_mutex_destroy(&p->forks[i].locker);
		i++;
	}
	free(p->forks);
	p->forks = NULL;
}

void	destroy_params(t_params *p)
{
	if (!p)
		return ;
	if (p->forks)
		destroy_forks(p);
	pthread_mutex_destroy(&p->g_print_locker);
	pthread_mutex_destroy(&p->g_state_locker);
	free(p);
}

long	ceil_div(long a, long b)
{
	if (b <= 0)
		return (LONG_MAX);
	return ((a + b - 1) / b);
}

int	should_live_ms(t_params *p)
{
	int	e;
	int	d;
	int	base;

	e = (int)(p->to_eat / 1000LL);
	d = (int)(p->to_die / 1000LL);
	base = e + (int)(p->to_sleep / 1000LL);
	if ((p->n % 2) == 0)
	{
		if (base < 2 * e)
			base = 2 * e;
	}
	else
	{
		if (base < 3 * e)
			base = 3 * e;
	}
	return (d > base + 50 / p->n);
}

long	get_cycle(long n, long t_die, long t_eat, long t_sleep)
{
	long	m;
	long	r;
	long	need;
	long	gap;

	if (n <= 0 || t_die < 0 || t_eat < 0 || t_sleep < 0)
		return (-1);
	m = n / 2;
	if (n < 2 || t_eat >= t_die || m == 0)
		return (0);
	r = ceil_div(n, m) * t_eat;
	need = t_eat + t_sleep;
	if (need >= t_die)
		return (0);
	gap = ceil_div(need, r) * r;
	if (gap < t_die)
	{
		return (1);
	}
	return (0);
}

int	setup_all(t_params *p)
{
	if (p->n < 1 || pthread_mutex_init(&p->g_state_locker, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&p->g_print_locker, NULL) != 0)
	{
		pthread_mutex_destroy(&p->g_state_locker);
		return (0);
	}
	if (p->n > 1)
	{
		p->forks = (t_forks *)malloc(sizeof(t_forks) * p->n);
		if (!p->forks)
		{
			pthread_mutex_destroy(&p->g_print_locker);
			pthread_mutex_destroy(&p->g_state_locker);
			return (0);
		}
		init_forks(p);
	}
	p->live = get_cycle(p->n, (long)(p->to_die / 1000LL), (long)(p->to_eat
				/ 1000LL), (long)(p->to_sleep / 1000LL));
	p->start_at = now_us() + 2000LL;
	return (1);
}

t_params	*init_params(int argc, char **argv)
{
	t_params	*p;

	if (argc < 5)
		return (NULL);
	p = (t_params *)malloc(sizeof(*p));
	if (!p)
		return (NULL);
	p->n = (int)ft_atoill(argv[1]);
	p->to_die = ft_atoill(argv[2]) * 1000LL;
	p->to_eat = ft_atoill(argv[3]) * 1000LL;
	p->to_sleep = ft_atoill(argv[4]) * 1000LL;
	p->must_eat = -1;
	if (argc >= 6)
		p->must_eat = (int)ft_atoill(argv[5]);
	p->g_state = 0;
	p->forks = NULL;
	if (!setup_all(p))
	{
		free(p);
		return (NULL);
	}
	return (p);
}
