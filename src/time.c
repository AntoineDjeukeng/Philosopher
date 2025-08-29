#include "philo.h"
#include <time.h>

/* current monotonic time in milliseconds */
long	now_ms(void)
{
	struct timespec	ts;

	clock_gettime(CLOCK_REALTIME, &ts);
	return ((ts.tv_sec * 1000L) + (ts.tv_nsec / 1000000L));
}

/* sleep ~ms milliseconds with low overshoot using nanosleep slices */
void	precise_msleep(long ms)
{
	struct timespec	req;
	long			end;
	long			remaining;

	if (ms <= 0)
		return ;
	end = now_ms() + ms;
	while (1)
	{
		remaining = end - now_ms();
		if (remaining <= 0)
			break ;
		if (remaining >= 5)
		{
			req.tv_sec = 0;
			req.tv_nsec = 2 * 1000 * 1000;
		}
		else
		{
			req.tv_sec = 0;
			req.tv_nsec = 200 * 1000;
		}
		nanosleep(&req, NULL);
	}
}
