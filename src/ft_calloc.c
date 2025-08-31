/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 07:28:00 by adjeuken          #+#    #+#             */
/*   Updated: 2025/08/30 17:56:06 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	ft_bzero_local(void *ptr, size_t n)
{
	volatile unsigned char	*v;
	size_t					i;

	v = (volatile unsigned char *)ptr;
	i = 0;
	while (i < n)
	{
		v[i] = 0;
		i++;
	}
}

void	*ft_calloc(size_t nmemb, size_t size)
{
	size_t	total;
	void	*ptr;

	if (nmemb && size > SIZE_MAX / nmemb)
		return (NULL);
	total = nmemb * size;
	ptr = malloc(total);
	if (!ptr)
		return (NULL);
	ft_bzero_local(ptr, total);
	return (ptr);
}
