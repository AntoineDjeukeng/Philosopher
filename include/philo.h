/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 16:59:24 by adjeuken          #+#    #+#             */
/*   Updated: 2025/08/30 18:27:21 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define _GNU_SOURCE
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

typedef struct s_logmsg
{
	int64_t			rel_ms;
	int				id;
	char			text[64];
	struct s_logmsg	*next;
}					t_logmsg;

typedef struct s_logger
{
	pthread_mutex_t	mx;
	t_logmsg		*head;
	t_logmsg		*tail;
	int				stop;
	int				killed;
	int64_t			start_ms;
}					t_logger;

typedef struct s_rules
{
	int				n;
	int				t_die;
	int				t_eat;
	int				t_sleep;
	int				t_meals;
	pthread_mutex_t	*forks;
}					t_rules;

typedef struct s_state
{
	t_rules			*rules;
	t_logger		*logger;
	pthread_mutex_t	state_mx;
	volatile int	stop;
}					t_state;

typedef struct s_philo
{
	int				id;
	int				left;
	int				right;
	long			meals;
	int64_t			last_meal_ms;
	t_state			*state;
}					t_philo;

typedef struct s_monitor
{
	t_philo			**philos;
	t_state			*state;
}					t_monitor;

typedef struct s_ctx
{
	t_rules			rules;
	t_logger		logger;
	t_state			state;
	t_philo			*philos;
	pthread_t		*threads;

	t_philo			**pp;
	t_monitor		*mon;
	pthread_t		th_log;
	pthread_t		th_mon;

}					t_ctx;

/* time */
void				*ft_calloc(size_t nmemb, size_t size);
int					ft_puterr(const char *s);
int64_t				now_ms(void);
void				sleep_1ms(void);
void				precise_sleep_ms(volatile int *stop_ptr,
						pthread_mutex_t *stop_mx, int ms);
int					stop_snapshot(t_state *state);
void				wait_until_stop(t_state *state);

/* parse/args */
int					ft_atoi_long(const char *s, int min, int max, int *out);
int					init_args(int ac, char **av, t_rules *rules);

/* init */
int					init_forks(t_rules *rules);
int					init_state(t_state *state, t_rules *rules, t_logger *lg);
void				init_philos(t_philo *ph, t_state *state);
void				logger_init(t_logger *lg);

/* logger */
void				log_post(t_logger *lg, int id, const char *msg);
void				logger_kill_with_death(t_logger *lg, int victim_id,
						int64_t rel_ms);
void				*logger_thread(void *arg);

void				logger_clear_and_set_death(t_logger *lg, int victim_id,
						int64_t rel_ms);
t_logmsg			*logmsg_make(int64_t rel, int id, const char *msg);
t_logmsg			*logger_pop(t_logger *lg, int *killed, int *stop);
/* philo utils */
void				safe_log(t_philo *p, const char *msg);
/* philo */
void				*philo_run(void *arg);
void				ph_desync(t_philo *p);
void				ph_think_guard(t_philo *p);
void				take_forks_blocking(t_philo *p);
void				ph_single_case(t_philo *p);
int					ph_eat_block(t_philo *p);
size_t				ft_strlcpy(char *dst, const char *src, size_t size);
int					ft_strcmp(const char *s1, const char *s2);
/* monitor */
void				*monitor_thread(void *arg);

/* lifecycle */
void				destroy_all(t_rules *rules, t_state *state, t_logger *lg);
int					run_threads(t_ctx *a);
int					app_init(int ac, char **av, t_ctx *a);
int					app_run_and_close(t_ctx *a);
