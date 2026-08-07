/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_elf.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexafer <alexafer@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/03 00:44:14 by alexafer          #+#    #+#             */
/*   Updated: 2026/08/07 22:56:56 by alexafer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody_woodpacker.h"

int	elf_seek(t_woody *wood, uint64_t seek_pos)
{
	if (seek_pos >= wood->file_len)
		return (-1);
	wood->file_pos = seek_pos;
	return (0);
}

void	*read_elf(t_woody *wood, int size)
{
	void	*buffer;

	if (size + wood->file_pos >= wood->file_len)
		return (0);
	buffer = &(wood->file)[wood->file_pos];
	wood->file_pos += size;
	return (buffer);
}
