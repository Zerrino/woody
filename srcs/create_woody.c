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
    if (wood->format == ELF32 && wood->file_len > (int)wood->header->elf32.e_shoff)
        return wood->header->elf32.e_shoff;
    else if (wood->format == ELF64 && wood->file_len > (int)wood->header->elf64.e_shoff)
        return wood->header->elf64.e_shoff;
    return wood->file_len;
}

static int  create32(t_woody *wood)
{
    void        *to_change;
    uint32_t    original_entry;
    uint32_t    new_addr;

    original_entry = wood->header->elf32.e_entry;
    wood->header->elf32.e_entry = wood->mem_start;
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
    to_change = ft_memnmem((void *)wood->stub, "\x42\x42\x42\x42\x42\x42\x42\x42", 8, wood->stub_size);
    if (to_change == 0)
    {
        wood->error = "invalid stub provided.";
        return (0);
    }

    new_addr = wood->header->elf64.e_entry - original_entry;
    ft_memcpy(to_change, &new_addr, sizeof(new_addr));


    to_encrypt = ft_memnmem((void *)wood->stub, "....WOODY........WOODY........WOODY....\n", 40, wood->stub_size);
    if (to_encrypt)
    {
        speack_encrypt(to_encrypt, 32);
    }

    return (1);
}

int create_woody(t_woody *wood)
{
    int         fd;
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
    return (1);
}
