/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 14:42:45 by adjeuken          #+#    #+#             */
/*   Updated: 2025/09/29 12:48:09 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <limits.h>
#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <semaphore.h>



typedef struct s_fork
{
	int	left_fork;
	int	right_fork;
	sem_t	lock;
}					t_fork;

typedef struct s_params
{
	int				n;
	int				state;
	long long		t_die;
	long long		t_eat;
	long long		t_sleep;
	int				live;
	int				must_eat;
	long long		start_at;
	pthread_mutex_t	state_mtx;
	pthread_mutex_t	print_mtx;
	t_fork			*forks;
	struct s_logger	*logger_ptr;
}					t_params;

typedef struct s_philo
{
	int				id;
	int				left;
	int				right;
	int				meals;
	long long		last_meal;
	int				has_left;
	int				has_right;
	t_params		*par;
}					t_philo;

typedef struct s_log_item
{
	char			*msg;
}					t_log_item;

typedef struct s_logger
{
	pthread_mutex_t	mtx;
	t_log_item		*q;
	int				cap;
	int				head;
	int				tail;
	int				count;
	int				stop;
	pthread_t		thread;
}					t_logger;

long long			now_us(void);
void				micro_sleep(long long rem_us);

char				*ll_to_str(long long n);
long long			parse_ll(const char *s);
int					str_len(const char *s);

void				print_msg(t_philo *p, const char *msg);
int					declare_death(t_philo *p, long long now);

long				ceil_div(long a, long b);
int					should_live_ms(t_params *p);
long				get_cycle(long n, long t_die, long t_eat, long t_sleep);
void				init_forks(t_params *p);
void				destroy_forks(t_params *p);
int					setup_all(t_params *p);
t_params			*init_params(int argc, char **argv);
void				destroy_params(t_params *p);

int					global_stop(t_params *par);
t_philo				*init_philosopher(int me, t_params *par);
void				return_forks(t_philo *p);
void				probe_one(t_philo *p, int fork_idx, int is_left);
int					take_forks(t_philo *p);
int					wait_until_us_or_die(t_philo *p, long long tgt);
int					philo_sleep_then_think(t_philo *p);
int					align_start(t_philo *p);
int					do_eat(t_philo *p);
void				*philosopher_routine(void *arg);

int					handle_single(t_params *par);
int					alloc_arrays(pthread_t **th, t_philo ***ph, int n);
int					start_threads(t_params *par, pthread_t *th, t_philo **ph);
void				cleanup(t_params *par, pthread_t *th, t_philo **ph,
						int started);

int					logger_init(t_logger *lg, int capacity);
int					logger_start(t_logger *lg);
void				logger_stop_and_join(t_logger *lg);
void				logger_destroy(t_logger *lg);
int					logger_enqueue(t_logger *lg, char *msg);

void				*ft_memcpy(void *dst, const void *src, size_t n);
