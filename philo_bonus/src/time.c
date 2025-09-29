/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 14:39:46 by adjeuken          #+#    #+#             */
/*   Updated: 2025/09/13 14:39:47 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

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

void	micro_sleep(long long rem)
{
	if (rem <= 150)
		return ;
	if (rem > 300)
		rem = 300;
	else if (rem < 0)
		rem = 0;
	usleep((useconds_t)rem);
}
