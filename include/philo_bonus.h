#ifndef PHILO_BONUS_H
# define PHILO_BONUS_H

# include <semaphore.h>
# include <sys/types.h>
# include <time.h>

/* same t_params as mandatory */
typedef struct s_params
{
	int   n;
	long  time_to_die;
	long  time_to_eat;
	long  time_to_sleep;
	int   must_eat;
}	t_params;

/* bonus uses processes, not threads */
typedef struct s_philo
{
	int      id;
	pid_t    pid;            // each philosopher is a child process
	long     last_meal_ms;
	int      meals_eaten;
	t_params *p;
}	t_philo;

typedef struct s_shared
{
	sem_t *forks;    // semaphore array (or a single semaphore count)
	sem_t *print;    // serialize logs
	sem_t *state;    // protect dead flag
	long   start_ms;
	int    dead;
}	t_shared;

#endif
