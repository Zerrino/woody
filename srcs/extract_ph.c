/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   extract_ph.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexafer <alexafer@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/02 22:46:09 by alexafer          #+#    #+#             */
/*   Updated: 2026/08/03 01:50:50 by alexafer         ###   ########.fr       */
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
	void	*ph;
	t_list	*new_ph;


	ph_offset = get_ph_offset(wood);
	ph_number = get_ph_number(wood);
	ph_size = get_ph_size(wood);

	if (elf_seek(wood, ph_offset) == -1)
		return (0);

	printf("ph number : %d, ph_offset : %d, ph_size : %d\n", ph_number, ph_offset, ph_size);
	i = 0;
	wood->pt_note = -1;
	while (i < ph_number)
	{
		ph = read_elf(wood, ph_size);
		if (!ph)
			return (0);
		new_ph = ft_lstnew(ph);
		if (!new_ph)
			return (0);
		printf("New segment : %d\n", i);
		if (wood->pt_note == -1 && ((wood->format == ELF32 && ((Elf32_Phdr *)ph)->p_type == PT_NOTE) ||
				(wood->pt_note == -1 && wood->format == ELF64 && ((Elf64_Phdr *)ph)->p_type == PT_NOTE)))
			wood->pt_note = i;
		ft_lstadd_back(&wood->elf_ph, new_ph);
		i++;
	}
	printf("pt_note : %d\n", wood->pt_note);
	return (1);
}
