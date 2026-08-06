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
    uint64_t    original_entry;

    fd = open("./woody", O_CREAT | O_WRONLY, 0755);
    if (fd < 0)
        return (0);

    original_entry = wood->header->elf64.e_entry;
    wood->header->elf64.e_entry = wood->mem_start;
    printf("original entry : 0x%lx\n", original_entry);
    printf("new      entry : 0x%lx\n", wood->header->elf64.e_entry);

    char	*ft_memnmem(const char *haystack, const char *needle, size_t needle_len, size_t len);
    printf("gap : %lx\n", original_entry - wood->header->elf64.e_entry);
    write(fd, wood->file, wood->file_len);
    write(fd, wood->stub, wood->stub_size);
    return (1);
}