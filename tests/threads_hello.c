#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>



typedef struct s_logmsg {
	int64_t         rel_ms;
	int             id;
	char            text[64];
	struct s_logmsg *next;
}	t_logmsg;

typedef struct s_logger {
	pthread_mutex_t mx;
	t_logmsg        *head;
	t_logmsg        *tail;
	int             stop;
	int             killed;
	int64_t         start_ms;
}	t_logger;

typedef struct s_rules {
	int             n;
	int             t_die;
	int             t_eat;
	int             t_sleep;
	int             t_meals;
	pthread_mutex_t *forks;
}	t_rules;

typedef struct s_state {
	t_rules         *rules;
	t_logger        *logger;
	pthread_mutex_t state_mx;
	volatile int    stop;
}	t_state;

typedef struct s_philo {
	int             id;
	int             left;
	int             right;
	long            meals;
	int64_t         last_meal_ms;
	t_state         *state;
}	t_philo;

typedef struct s_monitor {
	t_philo         **philos;
	t_state         *state;
}	t_monitor;

typedef struct s_ctx {
	t_rules         rules;
	t_logger        logger;
	t_state         state;
	t_philo         *philos;
	pthread_t       *threads;

	t_philo   **pp;
	t_monitor *mon;
	pthread_t th_log;
	pthread_t th_mon;

}	t_ctx;



static int64_t	now_ms(void)
{
	struct timespec	ts;
	int				r;
	int64_t			s;
	int64_t			ns;
	int64_t			ms;

	r = clock_gettime(CLOCK_MONOTONIC, &ts);
	if (r != 0)
		return (0);
	s = (int64_t)ts.tv_sec;
	ns = (int64_t)ts.tv_nsec;
	ms = s * 1000;
	ms += ns / 1000000;
	return (ms);
}

static void	sleep_1ms(void)
{
	struct timespec	ts;
	int				r;

	ts.tv_sec = 0;
	ts.tv_nsec = 1000000;
	r = nanosleep(&ts, NULL);
	if (r != 0)
	{
		ts.tv_sec = 0;
		ts.tv_nsec = 1000000;
		nanosleep(&ts, NULL);
	}
}

static void	precise_sleep_ms(volatile int *stop_ptr, pthread_mutex_t *stop_mx, int ms)
{
	int64_t	end;
	int		hit;

	end = now_ms() + ms;
	hit = 0;
	while (now_ms() < end)
	{
		if (stop_mx)
		{
			pthread_mutex_lock(stop_mx);
			hit = *stop_ptr;
			pthread_mutex_unlock(stop_mx);
		}
		else
			hit = *stop_ptr;
		if (hit)
			break ;
		sleep_1ms();
	}
}


static t_logmsg	*logmsg_make(int64_t rel, int id, const char *msg)
{
	t_logmsg	*m;
	size_t		n;

	m = (t_logmsg *)malloc(sizeof(*m));
	if (!m)
		return (NULL);
	m->rel_ms = rel;
	m->id = id;
	n = sizeof(m->text) - 1;
	strlcpy(m->text, msg, n);
	m->text[n] = '\0';
	m->next = NULL;
	return (m);
}
static int	stop_snapshot(t_state *sh)
{
	int	v;
	int	r;

	pthread_mutex_lock(&sh->state_mx);
	v = sh->stop;
	pthread_mutex_unlock(&sh->state_mx);
	r = 0;
	if (v)
		r = 1;
	return (r);
}

static void	wait_until_stop(t_state *sh)
{
	int	v;

	while (1)
	{
		pthread_mutex_lock(&sh->state_mx);
		v = sh->stop;
		pthread_mutex_unlock(&sh->state_mx);
		if (v)
			break ;
		sleep_1ms();
	}
}
static void	log_post(t_logger *lg, int id, const char *msg)
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


static void	safe_log(t_philo *p, const char *msg)
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


static void	ph_think_guard(t_philo *p)
{
	int			odd;
	int			delay;
	int64_t		now;
	int64_t		last;
	int64_t		rem;

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
		delay = (rem > 2) ? (int)(rem - 2) : 0;
	if (delay > 0)
		precise_sleep_ms(&p->state->stop, &p->state->state_mx, delay);
}

static void	logger_clear_and_set_death(t_logger *lg, int victim_id, int64_t rel_ms)
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
		strcpy(m->text, "died");
		m->next = NULL;
		lg->head = m;
		lg->tail = m;
	}
}

static void	logger_kill_with_death(t_logger *lg, int victim_id, int64_t rel_ms)
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



static t_logmsg	*logger_pop(t_logger *lg, int *killed, int *stop)
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

static void	*logger_thread(void *arg)
{
	t_logger	*lg;
	t_logmsg	*m;
	int			killed;
	int			stop;

	lg = (t_logger *)arg;
	while (1)
	{
		m = logger_pop(lg, &killed, &stop);
		if (!m)
		{
			if (stop)
				break ;
			sleep_1ms();
			continue ;
		}
		if (killed && strcmp(m->text, "died") != 0)
			free(m);
		else
		{
			printf("%lld %d %s\n", (long long)m->rel_ms, m->id, m->text);
			free(m);
		}
	}
	return (NULL);
}





static void	take_forks_blocking(t_philo *p)
{
	t_rules	*rules;
	int			a;
	int			b;

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

static void	ph_single_case(t_philo *p)
{
	t_rules	*rules;
	int64_t		now;
	int64_t		last;
	int64_t		rem;
	int			delay;

	rules = p->state->rules;
	safe_log(p, "is thinking");
	pthread_mutex_lock(&rules->forks[p->left]);
	safe_log(p, "has taken a fork");
	pthread_mutex_lock(&p->state->state_mx);
	last = p->last_meal_ms;
	pthread_mutex_unlock(&p->state->state_mx);
	now = now_ms();
	rem = rules->t_die - (now - last);
	delay = (rem > 2) ? (int)(rem - 2) : 0;
	if (delay > 0 && !stop_snapshot(p->state))
		precise_sleep_ms(&p->state->stop, &p->state->state_mx, delay);
	wait_until_stop(p->state);
	pthread_mutex_unlock(&rules->forks[p->left]);
}


static void	ph_desync(t_philo *p)
{
	t_rules	*rules;
	int			d;

	rules = p->state->rules;
	d = 0;
	if (rules->n % 2 == 0 && p->id % 2 == 0)
		d = 2;
	if (rules->n % 2 == 1 && p->id % 2 == 0)
		d = rules->t_eat / 2;
	if (d > 0)
		precise_sleep_ms(&p->state->stop, &p->state->state_mx, d);
}


static int	ph_eat_block(t_philo *p)
{
	t_rules	*rules;
	int			done;

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
static void	*philo_run(void *arg)
{
	t_philo		*p;

	p = (t_philo *)arg;
	if (p->state->rules->n == 1)
	{
		ph_single_case(p);
		return (NULL);
	}
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
		precise_sleep_ms(&p->state->stop, &p->state->state_mx, p->state->rules->t_sleep);
	}
	return (NULL);
}


static int	mon_all_full(t_monitor *ma, int *done)
{
	int	i;
	int	full;
	int	n;

	i = 0;
	full = 0;
	n = ma->state->rules->n;
	while (i < n)
	{
		if (!done[i] && (int)ma->philos[i]->meals >= ma->state->rules->t_meals)
			done[i] = 1;
		full += done[i];
		i++;
	}
	if (full == n)
		return (1);
	return (0);
}

static int	mon_check_dead(t_monitor *ma, int *vid, int64_t *rel)
{
	int			i;
	int			n;
	int64_t		t;
	int64_t		last;
	t_rules	*rules;

	rules = ma->state->rules;
	n = rules->n;
	i = 0;
	t = now_ms();
	while (i < n)
	{
		pthread_mutex_lock(&ma->state->state_mx);
		last = ma->philos[i]->last_meal_ms;
		pthread_mutex_unlock(&ma->state->state_mx);
		if (t - last > rules->t_die)
		{
			*vid = ma->philos[i]->id;
			*rel = (last + rules->t_die) - ma->state->logger->start_ms;
			return (1);
		}
		i++;
	}
	return (0);
}

static int	monitor_loop(t_monitor *ma, int *done)
{
	int		vid;
	int64_t	rel;

	while (!stop_snapshot(ma->state))
	{
		if (done && mon_all_full(ma, done))
		{
			pthread_mutex_lock(&ma->state->state_mx);
			ma->state->stop = 1;
			pthread_mutex_unlock(&ma->state->state_mx);
			return (0);
		}
		if (mon_check_dead(ma, &vid, &rel))
		{
			pthread_mutex_lock(&ma->state->state_mx);
			ma->state->stop = 1;
			pthread_mutex_unlock(&ma->state->state_mx);
			logger_kill_with_death(ma->state->logger, vid, rel);
			return (1);
		}
		sleep_1ms();
	}
	return (0);
}


static void	*monitor_thread(void *arg)
{
	t_monitor	*ma;
	int				*done;
	int				r;

	ma = (t_monitor *)arg;
	done = NULL;
	if (ma->state->rules->t_meals > 0)
	{
		done = (int *)calloc(ma->state->rules->n, sizeof(int));
		if (!done)
			return (NULL);
	}
	r = monitor_loop(ma, done);
    if (!r)
    {
        pthread_mutex_lock(&ma->state->logger->mx);
        ma->state->logger->stop = 1;
        pthread_mutex_unlock(&ma->state->logger->mx);
    }
	free(done);
	return (NULL);
}


static int	rulesrse_prefix(const char *s, int *i, int *sign)
{
	int	k;

	if (!s || !*s)
		return (0);
	k = 0;
	while (s[k] == ' ' || (s[k] >= 9 && s[k] <= 13))
		k++;
	*sign = 1;
	if (s[k] == '+' || s[k] == '-')
	{
		if (s[k] == '-')
			*sign = -1;
		k++;
	}
	if (s[k] < '0' || s[k] > '9')
		return (0);
	*i = k;
	return (1);
}

int	ft_atoi_long(const char *str, int min, int max, int *out)
{
	int		i;
	int		sign;
	long	res;

	if (!rulesrse_prefix(str, &i, &sign))
		return (write(2, "Invalid args\n", 13), 0);
	res = 0;
	while (str[i] >= '0' && str[i] <= '9')
	{
		res = res * 10 + (str[i] - '0');
		if (res > (long)max + (sign == -1))
			return (write(2, "Invalid args\n", 13), 0);
		i++;
	}
	if (str[i] != '\0')
		return (write(2, "Invalid args\n", 13), 0);
	if (sign == -1)
		res = -res;
	if (res < (long)min || res > (long)max)
		return (write(2, "Invalid args\n", 13), 0);
	*out = (int)res;
	return (1);
}


static int	init_args(int ac, char **av, t_rules *rules)
{
	if (ac < 5 || ac > 6)
	{
		fprintf(stderr, "Usage: %s N t_die t_eat t_sleep [t_meals]\n", av[0]);
		return (0);
	}
	if (!ft_atoi_long(av[1], 1, 1000000000, &rules->n))
		return (0);
	if (!ft_atoi_long(av[2], 0, 1000000000, &rules->t_die))
		return (0);
	if (!ft_atoi_long(av[3], 0, 1000000000, &rules->t_eat))
		return (0);
	if (!ft_atoi_long(av[4], 0, 1000000000, &rules->t_sleep))
		return (0);
	rules->t_meals = -1;
	if (ac == 6)
	{
		if (!ft_atoi_long(av[5], 1, 1000000000, &rules->t_meals))
			return (0);
	}
	return (1);
}




static int	init_forks(t_rules *rules)
{
	int	i;

	rules->forks = (pthread_mutex_t *)malloc(sizeof(*rules->forks) * (size_t)rules->n);
	if (!rules->forks)
		return (0);
	i = 0;
	while (i < rules->n)
	{
		pthread_mutex_init(&rules->forks[i], NULL);
		i++;
	}
	return (1);
}

static int	init_state(t_state *state, t_rules *rules, t_logger *lg)
{
	state->rules = rules;
	state->logger = lg;
	state->stop = 0;
	if (pthread_mutex_init(&state->state_mx, NULL) != 0)
		return (0);
	return (1);
}

static void	init_philos(t_philo *ph, t_state *state)
{
	int		i;
	int64_t	start;

	start = state->logger->start_ms;
	i = 0;
	while (i < state->rules->n)
	{
		ph[i].id = i + 1;
		ph[i].left = i;
		ph[i].right = (i + 1) % state->rules->n;
		ph[i].meals = 0;
		ph[i].last_meal_ms = start;
		ph[i].state = state;
		i++;
	}
}


static void	destroy_all(t_rules *rules, t_state *state, t_logger *lg)
{
	int	i;
    t_logmsg	*m;

	i = 0;
	while (i < rules->n)
	{
		pthread_mutex_destroy(&rules->forks[i]);
		i++;
	}
	free(rules->forks);
	pthread_mutex_destroy(&state->state_mx);
	while (lg->head)
	{
		m = lg->head;
		lg->head = m->next;
		free(m);
	}
	pthread_mutex_destroy(&lg->mx);
}

static int	start_threads(t_ctx *a)
{
	int	i;

	a->pp = (t_philo **)malloc(sizeof(*a->pp) * (size_t)a->rules.n);
	if (!a->pp)
		return (fprintf(stderr, "alloc failed\n"), 1);
	a->mon = (t_monitor *)malloc(sizeof(*a->mon));
	if (!a->mon)
		return (free(a->pp), fprintf(stderr, "alloc failed\n"), 1);
	pthread_create(&a->th_log, NULL, logger_thread, a->state.logger);
	i = 0;
	while (i < a->rules.n)
	{
		pthread_create(&a->threads[i], NULL, philo_run, &a->philos[i]);
		a->pp[i] = &a->philos[i];
		i++;
	}
	a->mon->philos = a->pp;
	a->mon->state = &a->state;
	pthread_create(&a->th_mon, NULL, monitor_thread, a->mon);
	return (0);
}


static int	run_threads(t_ctx *a)
{
	int	i;

	if (start_threads(a))
		return (1);
	i = 0;
	while (i < a->rules.n)
	{
		pthread_join(a->threads[i], NULL);
		i++;
	}
	pthread_join(a->th_mon, NULL);
	pthread_mutex_lock(&a->state.logger->mx);
	a->state.logger->stop = 1;
	pthread_mutex_unlock(&a->state.logger->mx);
	pthread_join(a->th_log, NULL);
	free(a->mon);
	free(a->pp);
	return (0);
}



static void	logger_init(t_logger *lg)
{
	pthread_mutex_init(&lg->mx, NULL);
	lg->head = NULL;
	lg->tail = NULL;
	lg->stop = 0;
	lg->killed = 0;
	lg->start_ms = now_ms();
	if (lg->start_ms < 0)
		lg->start_ms = 0;
}


static int	app_init(int ac, char **av, t_ctx *a)
{
	if (!init_args(ac, av, &a->rules))
		return (1);
	if (!init_forks(&a->rules))
		return (1);
	logger_init(&a->logger);
	if (!init_state(&a->state, &a->rules, &a->logger))
		return (1);
	a->philos = (t_philo *)calloc((size_t)a->rules.n, sizeof(*a->philos));
	a->threads = (pthread_t *)malloc(sizeof(*a->threads) * (size_t)a->rules.n);
	if (!a->philos || !a->threads)
		return (free(a->threads), free(a->philos), 1);
	init_philos(a->philos, &a->state);
	return (0);
}

static int	app_run_and_close(t_ctx *a)
{
	int	fail;

	fail = run_threads(a);
	free(a->threads);
	free(a->philos);
	destroy_all(&a->rules, &a->state, &a->logger);
	if (fail)
		return (1);
	return (0);
}

int	main(int ac, char **av)
{
	t_ctx	a;

	if (app_init(ac, (char **)av, &a))
		return (1);
	return (app_run_and_close(&a));
}

