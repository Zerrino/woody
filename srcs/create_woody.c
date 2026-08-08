 /* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   create_woody.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: reborn <reborn@42belgium.be>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/06 16:02:09 by reborn            #+#    #+#             */
/*   Updated: 2026/08/06 17:41:32 by reborn           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody_woodpacker.h"

uint64_t  get_writing_point(t_woody *wood)
{
    if (wood->format == ELF32 && wood->file_len > wood->e_shoff && wood->e_shoff != 0)
        return wood->e_shoff;
    else if (wood->format == ELF64 && wood->file_len > wood->e_shoff && wood->e_shoff != 0)
        return wood->e_shoff;
    return wood->file_len;
}

static int  create32(t_woody *wood)
{
    void        *to_change;
    uint32_t    original_entry;
    uint32_t    new_addr;

    original_entry = wood->header->elf32.e_entry;
    wood->header->elf32.e_entry = wood->mem_start;
    wood->header->elf32.e_shoff = 0;
    wood->header->elf32.e_shnum = 0;
    wood->header->elf32.e_shstrndx = 0;
    wood->header->elf32.e_shentsize = 0;
    to_change = ft_memnmem((void *)wood->stub, "\x42\x42\x42\x42", 4, wood->stub_size);
    if (to_change == 0)
    {
        wood->error = "invalid stub provided.";
        return (0);
    }
    new_addr = wood->header->elf32.e_entry - original_entry;
    ft_memcpy(to_change, &new_addr, sizeof(new_addr));
    return (1);
}

static int  create64(t_woody *wood)
{
    void        *to_change;
    void        *to_encrypt;
    uint64_t    original_entry;
    uint64_t    new_addr;

    original_entry = wood->header->elf64.e_entry;
    wood->header->elf64.e_entry = wood->mem_start;
    wood->header->elf64.e_shoff = 0;
    wood->header->elf64.e_shnum = 0;
    wood->header->elf64.e_shstrndx = 0;
    wood->header->elf64.e_shentsize = 0;

    to_change = ft_memnmem((void *)wood->stub, "\x42\x42\x42\x42\x42\x42\x42\x42", 8, wood->stub_size);
    if (to_change == 0)
    {
        wood->error = "invalid stub provided.";
        return (0);
    }
    new_addr = wood->header->elf64.e_entry - original_entry;
    ft_memcpy(to_change, &new_addr, sizeof(new_addr));
    to_encrypt = ft_memnmem((void *)wood->stub, "....WOODY....\n", 14, wood->stub_size);
    if (to_encrypt)
    {
        speack_encrypt(to_encrypt, 16);
    }
    return (1);
}

int create_woody(t_woody *wood)
{
    t_list          *lst;
    t_pt_encrypted  *pt;
    uint64_t        lst_len;
    uint64_t        offset;
    int             fd;

    fd = open("./woody", O_CREAT | O_WRONLY, 0755);
    if (fd < 0)
    {
        wood->error = "failed to open the woody file.";
        return (0);
    }
    if (wood->format == ELF32 && create32(wood) == 0)
        return (0);
    else if (wood->format == ELF64 && create64(wood) == 0)
        return (0);
    write(fd, wood->file, get_writing_point(wood));
    write(fd, wood->stub, wood->stub_size);
    lst = wood->pt_encrypted;
    lst_len = ft_lstsize(lst);
    write(fd, &lst_len, 8);

    while (lst)
    {
        pt = lst->content;
        offset = wood->header->elf64.e_entry - pt->memo_offset;
        write(fd, &offset, 8);
        write(fd, &pt->size, 8);
        lst = lst->next;
    }
    close(fd);
    return (1);
}
