/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_elf.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexafer <alexafer@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/03 00:44:14 by alexafer          #+#    #+#             */
/*   Updated: 2026/08/03 01:31:19 by alexafer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody_woodpacker.h"


void	*read_elf(t_woody *wood, int size)
{
	void	*buffer;

	if (size + wood->file_pos >= wood->file_len)
		return (0);
	buffer = &(wood->file)[wood->file_pos];
	wood->file_pos += size;
	return (buffer);
}
