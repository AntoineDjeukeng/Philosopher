#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>   
/* ---------- utils: time & atoi ---------- */

static long   now_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000L) + (tv.tv_usec / 1000L);
}

static void   msleep_precise(int ms) {
    long end = now_ms() + ms;
    while (now_ms() < end)
        usleep(1000);
}

static int    parse_prefix(const char *s, int *i, int *sign) {
    int k = 0;
    if (!s || !*s) return 0;
    while (s[k] == ' ' || (s[k] >= 9 && s[k] <= 13)) k++;
    *sign = 1;
    if (s[k] == '+' || s[k] == '-') { if (s[k] == '-') *sign = -1; k++; }
    if (s[k] < '0' || s[k] > '9') return 0;
    *i = k;
    return 1;
}

static int    ft_atoi_long(const char *str, int min, int max, int *out) {
    int i, sign; long res = 0;
    if (!parse_prefix(str, &i, &sign)) { write(2, "Invalid args\n", 13); return 0; }
    while (str[i] >= '0' && str[i] <= '9') {
        res = res * 10 + (str[i] - '0');
        if (res > (long)max + (sign == -1)) { write(2, "Invalid args\n", 13); return 0; }
        i++;
    }
    if (str[i] != '\0') { write(2, "Invalid args\n", 13); return 0; }
    if (sign == -1) res = -res;
    if (res < (long)min || res > (long)max) { write(2, "Invalid args\n", 13); return 0; }
    *out = (int)res; return 1;
}

/* ---------- config ---------- */

typedef struct s_rules {
    int n;
    int t_die;
    int t_eat;
    int t_sleep;
    int t_meals; /* -1 if unlimited */
} t_rules;

typedef struct s_ctx {
    t_rules rules;
    long    start_ms;
    sem_t  *sem_forks;  /* counting: N */
    sem_t  *sem_seats;  /* counting: N/2 to avoid deadlock */
    sem_t  *sem_print;  /* binary: 1; keep locked after "died" */
    /* names so we can unlink on exit */
    char    name_forks[64];
    char    name_seats[64];
    char    name_print[64];
} t_ctx;

static t_ctx G; /* inherited by children after fork */

/* build “/philo_<base>_<pid>” without sprintf */
static void   build_name(char *dst, const char *base) {
    int i = 0;
    pid_t p = getpid();
    char tmp[32]; int tlen = 0;
    dst[i++] = '/';
    while (*base && i < 60) dst[i++] = *base++;
    dst[i++] = '_';
    /* convert pid to decimal */
    if (p == 0) tmp[tlen++] = '0';
    while (p > 0 && tlen < (int)sizeof(tmp)) { tmp[tlen++] = '0' + (p % 10); p /= 10; }
    while (tlen-- > 0 && i < 63) dst[i++] = tmp[tlen];
    dst[i] = '\0';
}

/* ---------- printing (serialized) ---------- */

static void   print_state(int id, const char *msg, int is_death) {
    long rel = now_ms() - G.start_ms;
    sem_wait(G.sem_print);
    printf("%ld %d %s\n", rel, id, msg);
    if (!is_death) sem_post(G.sem_print);
    /* if death: do NOT post -> freeze printing exactly as subject expects */
}

/* ---------- child process (one philosopher) ---------- */

typedef struct s_local {
    int     id;
    int     meals;
    volatile long last_meal_ms;
} t_local;

static void  *watchdog_thread(void *arg) {
    t_local *L = (t_local*)arg;
    while (1) {
        long dt = now_ms() - L->last_meal_ms;
        if (dt > G.rules.t_die) {
            print_state(L->id, "died", 1);
            exit(1); /* non-zero -> parent kills the rest */
        }
        usleep(1000);
    }
    return NULL;
}

static void   take_forks(void) {
    /* avoid deadlock: only N/2 philosophers can attempt to grab forks */
    sem_wait(G.sem_seats);
    sem_wait(G.sem_forks);
    sem_wait(G.sem_forks);
}

static void   drop_forks(void) {
    sem_post(G.sem_forks);
    sem_post(G.sem_forks);
    sem_post(G.sem_seats);
}

static void   single_case(int id) {
    /* N == 1: can only take one fork and must die after t_die */
    print_state(id, "is thinking", 0);
    sem_wait(G.sem_forks);
    print_state(id, "has taken a fork", 0);
    msleep_precise(G.rules.t_die);
    print_state(id, "died", 1);
    exit(1);
}

static void   philo_loop(int id) {
    t_local L;
    pthread_t th;
    L.id = id;
    L.meals = 0;
    L.last_meal_ms = G.start_ms;

    if (G.rules.n == 1) single_case(id);

    /* small desync helps smoother traces; optional */
    if (id % 2 == 0) msleep_precise(G.rules.t_eat / 2);

    pthread_create(&th, NULL, watchdog_thread, &L);
    pthread_detach(th);

    while (1) {
        print_state(id, "is thinking", 0);

        take_forks();
        print_state(id, "has taken a fork", 0);
        print_state(id, "has taken a fork", 0);

        L.last_meal_ms = now_ms();
        print_state(id, "is eating", 0);
        msleep_precise(G.rules.t_eat);

        drop_forks();

        L.meals++;
        if (G.rules.t_meals > 0 && L.meals >= G.rules.t_meals)
            exit(0); /* finished successfully */

        print_state(id, "is sleeping", 0);
        msleep_precise(G.rules.t_sleep);
    }
}

/* ---------- parent orchestration ---------- */

static int    init_args(int ac, char **av, t_rules *r) {
    if (ac < 5 || ac > 6) {
        write(2, "Usage: ./philo_bonus N t_die t_eat t_sleep [t_meals]\n", 54);
        return 0;
    }
    if (!ft_atoi_long(av[1], 1, 1000000000, &r->n)) return 0;
    if (!ft_atoi_long(av[2], 0, 1000000000, &r->t_die)) return 0;
    if (!ft_atoi_long(av[3], 0, 1000000000, &r->t_eat)) return 0;
    if (!ft_atoi_long(av[4], 0, 1000000000, &r->t_sleep)) return 0;
    r->t_meals = -1;
    if (ac == 6 && !ft_atoi_long(av[5], 1, 1000000000, &r->t_meals)) return 0;
    return 1;
}

static int    sems_open(void) {
    /* ensure unique names per run */
    build_name(G.name_forks, "philo_forks");
    build_name(G.name_seats, "philo_seats");
    build_name(G.name_print, "philo_print");
    /* cleanup any stale semaphores from previous crashed runs */
    sem_unlink(G.name_forks);
    sem_unlink(G.name_seats);
    sem_unlink(G.name_print);

    G.sem_forks = sem_open(G.name_forks, O_CREAT, 0644, G.rules.n);
    G.sem_seats = sem_open(G.name_seats, O_CREAT, 0644, (G.rules.n > 1) ? (G.rules.n / 2) : 0);
    G.sem_print = sem_open(G.name_print, O_CREAT, 0644, 1);
    return (G.sem_forks != SEM_FAILED && G.sem_seats != SEM_FAILED && G.sem_print != SEM_FAILED);
}

static void   sems_close_unlink(void) {
    if (G.sem_forks != SEM_FAILED) sem_close(G.sem_forks);
    if (G.sem_seats != SEM_FAILED) sem_close(G.sem_seats);
    if (G.sem_print != SEM_FAILED) sem_close(G.sem_print);
    sem_unlink(G.name_forks);
    sem_unlink(G.name_seats);
    sem_unlink(G.name_print);
}

int main(int ac, char **av) {
    pid_t *pids = NULL;
    int i, alive, status, any_death = 0;

    memset(&G, 0, sizeof(G));
    if (!init_args(ac, av, &G.rules)) return 1;
    if (!sems_open()) { write(2, "sem_open failed\n", 16); return 1; }

    G.start_ms = now_ms();
    pids = (pid_t*)malloc(sizeof(pid_t) * (size_t)G.rules.n);
    if (!pids) { write(2, "malloc failed\n", 14); sems_close_unlink(); return 1; }

    /* spawn N philosopher processes */
    for (i = 0; i < G.rules.n; i++) {
        pid_t pid = fork();
        if (pid < 0) { write(2, "fork failed\n", 12); any_death = 1; break; }
        if (pid == 0) { /* child */
            philo_loop(i + 1);
            /* never returns */
        }
        pids[i] = pid;
    }
    alive = G.rules.n;

    /* parent: wait until a death (exit code 1) or all finished (all exit 0) */
    while (alive > 0) {
        pid_t who = waitpid(-1, &status, 0);
        if (who < 0) break;
        alive--;
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            any_death = 1;
            break;
        }
        /* if WIFSIGNALED, treat as death as well */
        if (WIFSIGNALED(status)) { any_death = 1; break; }
    }

    if (any_death) {
        /* kill everyone still alive */
        for (i = 0; i < G.rules.n; i++)
            if (pids[i] > 0) kill(pids[i], SIGKILL);
        /* ensure print semaphore stays locked (death already printed by culprit) */
    }

    free(pids);
    sems_close_unlink();
    return any_death ? 1 : 0;
}
