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


int create_woody(t_woody *wood)
{
    int         fd;
    void        *to_change;
    uint64_t     new_addr;
    uint64_t    original_entry;

    fd = open("./woody", O_CREAT | O_WRONLY, 0755);
    if (fd < 0)
        return (0);

    original_entry = wood->header->elf64.e_entry;
    wood->header->elf64.e_entry = wood->mem_start;
    to_change = ft_memnmem((void *)wood->stub, "\x42\x42\x42\x42\x42\x42\x42\x42", 8, wood->stub_size);
    new_addr = wood->header->elf64.e_entry - original_entry;
    ft_memcpy(to_change, &new_addr, sizeof(new_addr));
    write(fd, wood->file, wood->file_len);
    write(fd, wood->stub, wood->stub_size);
    return (1);
}
