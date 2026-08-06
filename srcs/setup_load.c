/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_load.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: reborn <reborn@42belgium.be>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/06 15:56:29 by reborn            #+#    #+#             */
/*   Updated: 2026/08/06 16:32:09 by reborn           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody_woodpacker.h"

static uint64_t rounding_up(uint64_t mem)
{
    return ((mem & ~0xfff) + 0x1000);
}

static void    setup_load32(t_woody *wood, Elf32_Phdr *pt_note)
{
    (void)wood;
    (void)pt_note;
}

static void    setup_load64(t_woody *wood, Elf64_Phdr *pt_note)
{
    (void)wood;
    pt_note->p_type = 1;
    pt_note->p_flags = PF_X | PF_R;
    pt_note->p_offset = wood->file_len;
    pt_note->p_paddr = wood->file_len;
    pt_note->p_vaddr = rounding_up(wood->biggest_mem_used) + (wood->file_len & 0xfff);
}

void    setup_load(t_woody *wood, void *pt_note)
{
    if (wood->format == ELF32)
        setup_load32(wood, pt_note);
    else
        setup_load64(wood, pt_note);
}