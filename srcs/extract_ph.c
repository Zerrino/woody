/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   extract_ph.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: reborn <reborn@42belgium.be>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/02 22:46:09 by alexafer          #+#    #+#             */
/*   Updated: 2026/08/06 16:24:02 by reborn           ###   ########.fr       */
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

static uint64_t	get_biggest_mem(t_woody *woody, void *ph)
{
	if (woody->format == ELF32)
		return ((uint64_t)((Elf32_Phdr *)ph)->p_vaddr + ((Elf32_Phdr *)ph)->p_memsz);
	return ((uint64_t)((Elf64_Phdr *)ph)->p_vaddr + ((Elf64_Phdr *)ph)->p_memsz);
}

uint64_t	get_program_entry(t_woody *woody)
{
	if (woody->format == ELF32)
		return ((uint64_t)woody->header->elf32.e_entry);
	return (woody->header->elf64.e_entry);
}

int	extract_ph(t_woody *wood)
{
	int			i;
	int			ph_offset;
	int			ph_number;
	int			ph_size;
	void		*ph;
	t_list		*new_ph;

	ph_offset = get_ph_offset(wood);
	ph_number = get_ph_number(wood);
	ph_size = get_ph_size(wood);

	if (elf_seek(wood, ph_offset) == -1)
		return (0);

	printf("ph number : %d, ph_offset : %d, ph_size : %d\n", ph_number, ph_offset, ph_size);
	i = 0;
	while (i < ph_number)
	{
		ph = read_elf(wood, ph_size);
		if (!ph)
			return (0);
		new_ph = ft_lstnew(ph);
		if (!new_ph)
			return (0);
		if (wood->pt_note == 0 && ((wood->format == ELF32 && ((Elf32_Phdr *)ph)->p_type == PT_NOTE) ||
				(wood->pt_note == 0 && wood->format == ELF64 && ((Elf64_Phdr *)ph)->p_type == PT_NOTE)))
			wood->pt_note = ph;
		if (wood->biggest_mem_used < get_biggest_mem(wood, ph))
			wood->biggest_mem_used = get_biggest_mem(wood, ph);
		ft_lstadd_back(&wood->elf_ph, new_ph);
		i++;
	}
	printf("biggest : 0x%lx\n", wood->biggest_mem_used);
	setup_load(wood, wood->pt_note);

	return (1);
}
