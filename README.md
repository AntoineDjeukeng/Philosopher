````markdown
# Dining Philosophers (pthread)

This project implements the classic **Dining Philosophers problem** in C using
`pthread` threads.
Two independent implementations are provided:

- **Mutex version** — uses `pthread_mutex_t` for forks, state, and printing.
- **Semaphore version** — uses POSIX semaphores (`sem_t*`) for synchronization.

Both versions use **microsecond timing** for accuracy and a **background logger
thread** to serialize output.

The code is split into small modules under `src/` with a single public header
under `include/`. A `Makefile` handles the build.

> **Note:** The old `new.c` / `new.h` in the root are not used by the build.
> Only the modularized code under `src/` and `include/` is compiled.

---

# Build

```sh
make        # build philo (select default version in Makefile)
make clean  # remove objects
make re     # clean + rebuild
````

Depending on your setup you may have separate targets like:

```sh
make mutex
make sem
```

---

# Usage

```sh
./philo N t_die t_eat t_sleep [must_eat]
```

* All times are in **milliseconds**.

* Example:

  ```sh
  ./philo 5 800 200 200 3
  ```

* With `must_eat` specified, the simulation ends once every philosopher
  has eaten at least that many times.

---

# Project Variants

## Mutex Version

* **Synchronization:**
  Uses `pthread_mutex_t` for each fork, plus dedicated mutexes for printing
  and global state.

* **Forks:**
  Stored as an array of `t_fork { owner, mtx }`.
  Access controlled by locking/unlocking the mutex.

* **Logger:**
  Uses a queue protected by a `pthread_mutex_t`.

* **Initialization:**
  `init_mutexes_and_forks()` sets up forks and global mutexes.

* **Deadlock avoidance:**
  Fork ownership and ordered locking strategy prevent deadlocks.

### Mutex Directory Layout

* **include/philo.h** — types + prototypes
* **src/**

  * `main.c`, `params_core.c`, `params_init.c`, `start_threads.c`
  * `philo_lifecycle.c`, `philo_core.c`, `philo_forks.c`, `forks.c`
  * `print.c`, `logger_api.c`, `logger.c`
  * `time.c`, `num.c`, `io.c`, `mem.c`

### Mutex Functions per File

| File                  | Functions                                                                                           |
| --------------------- | --------------------------------------------------------------------------------------------------- |
| **forks.o**           | `destroy_forks`, `init_forks`                                                                       |
| **io.o**              | `putstr_s`, `str_len`, `util_put_ms_id_msg`                                                         |
| **logger_api.o**      | `logger_enqueue`, `logger_init`, `logger_start`                                                     |
| **logger.o**          | `logger_destroy`, `logger_drain_locked`, `logger_stop_and_join`, `logger_take_one`, `logger_thread` |
| **main.o**            | `alloc_arrays`, `cleanup`, `handle_single`, `main`, `run_program`                                   |
| **mem.o**             | `ft_memcpy`                                                                                         |
| **num.o**             | `count_digits_ull`, `ll_to_str`, `parse_ll`, `parse_sign`, `set_cutoffs`                            |
| **params_core.o**     | `ceil_div`, `get_cycle`, `should_live_ms`                                                           |
| **params_init.o**     | `destroy_params`, `init_logger_wrap`, `init_mutexes_and_forks`, `init_params`, `setup_all`          |
| **philo_core.o**      | `global_stop`, `init_philosopher`, `wait_until_us_or_die`                                           |
| **philo_forks.o**     | `probe_one`, `return_forks`, `take_forks`                                                           |
| **philo_lifecycle.o** | `align_start`, `do_eat`, `philo_sleep_then_think`, `philosopher_routine`                            |
| **print.o**           | `build_log_line`, `compose_line`, `declare_death`, `print_msg`                                      |
| **start_threads.o**   | `start_threads`                                                                                     |
| **time.o**            | `micro_sleep`, `now_us`                                                                             |

---

## Semaphore Version

* **Synchronization:**
  Uses POSIX semaphores (`sem_t*`) for forks, printing, and state control.

* **Forks:**
  Stored as `t_fork { left_fork, right_fork, lock }` with a semaphore pointer.

* **Logger:**
  Uses a queue protected by a semaphore.

* **Initialization:**
  `init_sems_and_forks()` sets up forks and named semaphores.

* **Helpers:**
  Extra module `sem_utils.c` provides `sem_name_from_ptr()`, `append_hex()`,
  `append_uint()`.

* **Deadlock avoidance:**
  Similar strategy, but using semaphore acquisition/release.

### Semaphore Directory Layout

* **include/philo.h** — types + prototypes
* **src/**

  * `main.c`, `params_core.c`, `params_init.c`, `params_init_help.c`, `start_threads.c`
  * `philo_lifecycle.c`, `philo_core.c`, `philo_forks.c`, `forks.c`
  * `print.c`, `logger_api.c`, `logger.c`
  * `time.c`, `num.c`, `io.c`, `mem.c`, `sem_utils.c`

### Semaphore Functions per File

| File                   | Functions                                                                                                   |
| ---------------------- | ----------------------------------------------------------------------------------------------------------- |
| **forks.o**            | `assign_owners`, `destroy_forks`, `ft_fail`, `init_forks`                                                   |
| **io.o**               | `putstr_s`, `str_len`, `util_put_ms_id_msg`                                                                 |
| **logger_api.o**       | `logger_enqueue`, `logger_init`, `logger_start`, `logger_take_one`, `logger_try_enqueue`                    |
| **logger.o**           | `logger_destroy`, `logger_drain_and_unlock`, `logger_drain_locked`, `logger_stop_and_join`, `logger_thread` |
| **main.o**             | `alloc_arrays`, `cleanup`, `handle_single`, `main`, `run_program`                                           |
| **mem.o**              | `ft_memcpy`                                                                                                 |
| **num.o**              | `count_digits_ull`, `ll_to_str`, `parse_ll`, `parse_sign`, `set_cutoffs`                                    |
| **params_core.o**      | `ceil_div`, `get_cycle`, `should_live_ms`                                                                   |
| **params_init_help.o** | `close_params_sems`, `open_param_sem`                                                                       |
| **params_init.o**      | `destroy_params`, `init_logger_wrap`, `init_params`, `init_sems_and_forks`, `setup_all`                     |
| **philo_core.o**       | `global_stop`, `init_philosopher`, `wait_until_us_or_die`                                                   |
| **philo_forks.o**      | `probe_one`, `return_forks`, `take_forks`                                                                   |
| **philo_lifecycle.o**  | `align_start`, `do_eat`, `philo_sleep_then_think`, `philosopher_routine`                                    |
| **print.o**            | `build_log_line`, `compose_line`, `declare_death`, `mark_death_and_stop_logger`, `print_msg`                |
| **sem_utils.o**        | `append_hex`, `append_uint`, `sem_name_from_ptr`                                                            |
| **start_threads.o**    | `start_threads`                                                                                             |
| **time.o**             | `micro_sleep`, `now_us`                                                                                     |

---

# Key Differences

| Aspect             | Mutex Version                      | Semaphore Version                     |
| ------------------ | ---------------------------------- | ------------------------------------- |
| Forks              | `pthread_mutex_t` per fork         | `sem_t*` lock per fork                |
| Global state/print | Mutexes (`state_mtx`, `print_mtx`) | Semaphores (`state_sem`, `print_sem`) |
| Fork init          | `init_mutexes_and_forks()`         | `init_sems_and_forks()`               |
| Extra helpers      | –                                  | `sem_utils.c`                         |
| Logger sync        | Protected by mutex                 | Protected by semaphore                |

---

# Example Output

```
0 1 has taken a fork
0 1 is eating
200 1 is sleeping
400 1 is thinking
...
800 1 died
```

---

# Notes

* Philosopher indices are **zero-based** in output.
* For `N == 1`, the program prints a fork acquisition then death at `t_die`.
* Both implementations avoid deadlocks and guarantee correct timing.
* The **logger thread** guarantees ordered, non-overlapping prints.

```
