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
	// Bounds check: don't seek past end of file
	if (seek_pos >= wood->file_len)
		return (-1);
	
	// Update file cursor to desired position
	wood->file_pos = seek_pos;
	return (0);
}

void	*read_elf(t_woody *wood, int size)
{
	void	*buffer;

	// Bounds check: ensure we don't read past end of mapped file
	// Prevents buffer overread attacks or malformed input
	if (size + wood->file_pos >= wood->file_len)
		return (0);
	
	// Return pointer into mmap'd file buffer at current position
	// Note: This returns a DIRECT reference into the file memory,
	// NOT a copy - must be careful not to outlive the mmap
	buffer = &(wood->file)[wood->file_pos];
	wood->file_pos += size;
	return (buffer);
}
