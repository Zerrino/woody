/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   extract_ph.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexafer <alexafer@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/02 22:46:09 by alexafer          #+#    #+#             */
/*   Updated: 2026/08/08 01:39:32 by alexafer         ###   ########.fr       */
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

static int	encrypt_pt_load64(t_woody *wood, void *ph)
{
	t_list			*new_lst;
	t_pt_encrypted	*new_pt;

	new_pt = (t_pt_encrypted *)malloc(sizeof(t_pt_encrypted));
	if (!new_pt)
	{
		error:
		wood->error = "failed to malloc.";
		return (0);
	}
	new_pt->size = ((Elf64_Phdr *)ph)->p_filesz & ~0xf;
	new_pt->size /= 4;
	new_pt->size &= ~0xf;
	new_pt->offset = ((Elf64_Phdr *)ph)->p_offset + ((Elf64_Phdr *)ph)->p_filesz - new_pt->size;
	new_lst = ft_lstnew(new_pt);
	if (!new_lst)
	{
		free(new_pt);
		goto error;
	}
	ft_lstadd_back(&wood->pt_encrypted, new_lst);
	printf("offset : 0x%lX, size : 0x%lX\n", new_pt->offset, new_pt->size);
	speack_encrypt((char *)&wood->file[new_pt->offset], new_pt->size);
	return (1);
}

int	extract_ph(t_woody *wood)
{
	int				i;
	int				ph_offset;
	int				ph_number;
	int				ph_size;
	void			*ph;

	ph_offset = get_ph_offset(wood);
	ph_number = get_ph_number(wood);
	ph_size = get_ph_size(wood);

	if (elf_seek(wood, ph_offset) == -1)
	{
		wood->error = "invalid elf file.";
		return (0);
	}
	i = 0;
	while (i < ph_number)
	{
		ph = read_elf(wood, ph_size);
		if (!ph)
		{
			wood->error = "failed to read the program header.";
			return (0);
		}
		if (wood->pt_note == 0 && ((wood->format == ELF32 && ((Elf32_Phdr *)ph)->p_type == PT_NOTE) ||
				(wood->pt_note == 0 && wood->format == ELF64 && ((Elf64_Phdr *)ph)->p_type == PT_NOTE)))
			wood->pt_note = ph;
		if (wood->biggest_mem_used < get_biggest_mem(wood, ph))
			wood->biggest_mem_used = get_biggest_mem(wood, ph);
		if (wood->format == ELF32 && ((Elf32_Phdr *)ph)->p_type == PT_LOAD
				&& ((Elf32_Phdr *)ph)->p_flags & PF_X)
		{
		}
		else if (wood->format == ELF64 && ((Elf64_Phdr *)ph)->p_type == PT_LOAD
				&& ((Elf64_Phdr *)ph)->p_flags & PF_X)
		{
			if (encrypt_pt_load64(wood, ph) == 0)
				return (0);
			((Elf64_Phdr *)ph)->p_flags |= PF_W;
		}
		i++;
	}
	if (wood->pt_note == 0)
	{
		wood->error = "failed to pack.";
		return (0);
	}
	setup_load(wood, wood->pt_note);
	return (1);
}
