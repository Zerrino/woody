/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   extract_header.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexafer <alexafer@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/02 20:33:04 by alexafer          #+#    #+#             */
/*   Updated: 2026/08/07 22:55:24 by alexafer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody_woodpacker.h"

int	extract_header(t_woody *woody)
{
	// Validate ELF format
	woody->header = read_elf(woody, 0x40);
	if (!woody->header || ft_memcmp(woody->header, "\x7F" "ELF", 4))
	{
		woody->error = "file isn't an valid elf file.";
		return (0);
	}
	woody->format = ((char *)woody->header)[EI_CLASS];

	if (woody->format == ELF32)
		woody->e_shoff = woody->header->elf32.e_shoff;
	else
		woody->e_shoff = woody->header->elf64.e_shoff;
	return (1);
}
