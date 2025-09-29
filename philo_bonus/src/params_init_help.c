/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   params_init_help.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adjeuken <adjeuken@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 14:37:14 by adjeuken          #+#    #+#             */
/*   Updated: 2025/09/29 19:20:04 by adjeuken         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <fcntl.h>

int	open_param_sem(sem_t **dst, t_params *p, char tag)
{
	char	name[32];

	sem_name_from_ptr(name, tag, (uintptr_t)p, -1);
	sem_unlink(name);
	*dst = sem_open(name, O_CREAT | O_EXCL, 0600, 1);
	if (*dst == SEM_FAILED)
		return (0);
	sem_unlink(name);
	return (1);
}

void	close_params_sems(t_params *p)
{
	if (!p)
		return ;
	if (p->print_sem && p->print_sem != SEM_FAILED)
		sem_close(p->print_sem);
	p->print_sem = NULL;
	if (p->state_sem && p->state_sem != SEM_FAILED)
		sem_close(p->state_sem);
	p->state_sem = NULL;
}
