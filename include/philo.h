#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdint.h>
// # include <time.h>
// #include <linux/time.h>

#define _POSIX_C_SOURCE 200809L
#include <time.h>


/* -------------------- Parameters -------------------- */
typedef struct s_params
{
	int   n;                 // number of philosophers
	long  time_to_die;       // ms
	long  time_to_eat;       // ms
	long  time_to_sleep;     // ms
	long  second_timeout_ms; // max wait for second fork before retry
	long  retry_backoff_ms;  // backoff before retry
	int   must_eat;          // -1 if infinite
}	t_params;

/* -------------------- Forward declarations -------------------- */
typedef struct s_philo	t_philo;

/* -------------------- Shared resources -------------------- */
typedef struct s_shared
{
	pthread_mutex_t *forks;   // array of n mutexes
	pthread_mutex_t  print;   // serialize logging
	pthread_mutex_t  state;   // protect dead flag & last_meal
	long             start_ms;
	int              dead;    // 1 if someone died
	t_philo         *philos;  // pointer to all philosophers
}	t_shared;

/* -------------------- Philosopher -------------------- */
struct s_philo
{
	int              id;            // 1..n
	int              left;          // fork index
	int              right;         // fork index
	long             last_meal_ms;  // last eating time (ms)
	int              meals_eaten;   // count meals (for must_eat)
	pthread_t        thread;
	t_params        *p;
	t_shared        *s;
};

/* -------------------- Function Prototypes -------------------- */

/* parse.c */
int		parse_args(int argc, char **argv, t_params *p);

/* time.c */
long	now_ms(void);
void	precise_msleep(long ms);

/* log.c */
void	log_state(t_philo *ph, const char *msg);

/* forks.c */
int		try_take_both(t_philo *ph);
void	put_both(t_philo *ph);

/* philo.c */
void	*philo_main(void *arg);

/* watchdog.c */
void	*watchdog_main(void *arg);

/* sim.c */
int		start_sim(const t_params *p);

/* cleanup.c */
void	cleanup(t_params *p, t_shared *s);
int	ft_atoi_long(const char *str, long *out);
#endif
