/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_load.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexafer <alexafer@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/06 15:56:29 by reborn            #+#    #+#             */
/*   Updated: 2026/08/08 01:49:37 by alexafer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody_woodpacker.h"

static uint64_t rounding_up(uint64_t mem)
{
    return ((mem & ~0xfff) + 0x1000);
}

static void    setup_load32(t_woody *wood, Elf32_Phdr *pt_note)
{
    pt_note->p_type = 1;
    pt_note->p_flags = PF_X | PF_R | PF_W;
    pt_note->p_offset = get_writing_point(wood);
    pt_note->p_paddr = get_writing_point(wood);
    pt_note->p_vaddr = rounding_up(wood->biggest_mem_used) + (get_writing_point(wood) & 0xfff);
    pt_note->p_memsz = wood->stub_size;
    pt_note->p_filesz = wood->stub_size;
    pt_note->p_align = 0x1000;
    wood->mem_start = pt_note->p_vaddr;
}

static void    setup_load64(t_woody *wood, Elf64_Phdr *pt_note)
{
    printf("here! %d\n", ft_lstsize(wood->pt_encrypted));
    pt_note->p_type = 1;
    pt_note->p_flags = PF_X | PF_R | PF_W;
    pt_note->p_offset = get_writing_point(wood);
    pt_note->p_paddr = get_writing_point(wood);
    pt_note->p_vaddr = rounding_up(wood->biggest_mem_used) + (get_writing_point(wood) & 0xfff);
    pt_note->p_memsz = wood->stub_size;// + 0x8 + (ft_lstsize(wood->pt_encrypted) * 0x10);
    pt_note->p_filesz = wood->stub_size;// + 0x8 + (ft_lstsize(wood->pt_encrypted) * 0x10);
    pt_note->p_align = 0x1000;
    wood->mem_start = pt_note->p_vaddr;
}

void    setup_load(t_woody *wood, void *pt_note)
{
    if (wood->format == ELF32)
        setup_load32(wood, pt_note);
    else
        setup_load64(wood, pt_note);
}
