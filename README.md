# Dining Philosophers (pthread)

This is a small C project that implements the Dining Philosophers problem using `pthread` mutexes and timing logic. The code is organized into modules under `src/` with a public header in `include/` and a `Makefile` to build.

Note: The original monolithic files `new.c` and `new.h` in the root are left untouched. The project build uses only the code under `src/` and `include/`.

## Build

- Build: `make`
- Clean objects: `make clean`
- Clean and rebuild: `make re`
- Run a sample: `make run`

## Usage

```
./philo N t_die t_eat t_sleep [must_eat]
```
- All times are in milliseconds.
- Example: `./philo 5 800 200 200 3`

## Structure

- `include/philo.h`: Public API and types
- `src/time.c`: Time & sleeping helpers
- `src/util.c`: String/number utilities
- `src/print.c`: Synchronized printing and death reporting
- `src/params.c`: Parameter parsing, fork setup/teardown, scheduling heuristics
- `src/philo.c`: Philosopher routines and fork acquisition logic
- `src/main.c`: Program entry, thread orchestration

## Notes

- Philosopher indices are zero-based in output.
- For `N == 1`, the program prints a fork acquisition then death at `t_die`.
- The fork ownership scheme avoids deadlocks by passing ownership after eating.

