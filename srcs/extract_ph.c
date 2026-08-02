/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   extract_ph.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexafer <alexafer@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/02 22:46:09 by alexafer          #+#    #+#             */
/*   Updated: 2026/08/03 01:37:24 by alexafer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody_woodpacker.h"

static int	get_ph_number(t_woody *woody)
{
	if (woody->format == ELF32)
		return (woody->header->elf32.e_phnum);
	return (woody->header->elf64.e_phnum);
}

static int	get_ph_size(t_woody *woody)
{
	if (woody->format == ELF32)
		return (woody->header->elf32.e_phentsize);
	return (woody->header->elf64.e_phentsize);
}

static int	get_ph_offset(t_woody *woody)
{
	if (woody->format == ELF32)
		return (woody->header->elf32.e_phoff);
	return (woody->header->elf64.e_phoff);
}

int	extract_ph(t_woody *wood)
{
	int		i;
	int		ph_offset;
	int		ph_number;
	int		ph_size;


	ph_offset = get_ph_offset(wood);
	ph_number = get_ph_number(wood);
	ph_size = get_ph_size(wood);

	printf("ph number : %d, ph_offset : %d, ph_size : %d\n", ph_number, ph_offset, ph_size);
	i = 0;
	while (i < ph_number)
	{

		i++;
	}
	return (1);
}
