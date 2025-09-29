/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sem_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken <adjeuken@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 17:40:00 by adjeuken          #+#    #+#             */
/*   Updated: 2025/09/29 19:26:53 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	append_hex(char *dst, int *len, uintptr_t value)
{
	char	buf[sizeof(uintptr_t) * 2];
	int		pos;

	pos = 0;
	if (value == 0)
	{
		dst[(*len)++] = '0';
		return ;
	}
	while (value > 0 && pos < (int) sizeof(buf))
	{
		buf[pos++] = (char)((value & 0xFULL) + '0');
		if (buf[pos - 1] > '9')
			buf[pos - 1] = (char)('a' + (buf[pos - 1] - '0' - 10));
		value >>= 4;
	}
	while (pos-- > 0)
		dst[(*len)++] = buf[pos];
}

static void	append_uint(char *dst, int *len, unsigned int value)
{
	char	buf[10];
	int		pos;

	pos = 0;
	if (value == 0)
	{
		dst[(*len)++] = '0';
		return ;
	}
	while (value > 0 && pos < (int) sizeof(buf))
	{
		buf[pos++] = (char)('0' + (value % 10U));
		value /= 10U;
	}
	while (pos-- > 0)
		dst[(*len)++] = buf[pos];
}

void	sem_name_from_ptr(char *dst, char tag, uintptr_t key, int index)
{
	const char	prefix[] = "/philo_";
	int			len;
	int			i;

	len = 0;
	i = 0;
	while (prefix[i] != '\0')
	{
		dst[len++] = prefix[i++];
	}
	dst[len++] = tag;
	dst[len++] = '_';
	append_hex(dst, &len, key);
	if (index >= 0)
	{
		dst[len++] = '_';
		append_uint(dst, &len, (unsigned int)index);
	}
	dst[len] = '\0';
}
