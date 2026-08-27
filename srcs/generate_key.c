/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   generate_key.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: reborn <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 15:20:11 by reborn            #+#    #+#             */
/*   Updated: 2026/08/27 15:20:13 by reborn           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody_woodpacker.h"

int generate_key(t_woody *wood)
{
    int     fd;
    ssize_t ret;
    size_t  total;

    fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0)
        goto error;
    

    total = 0;
    while (total < sizeof(uint64_t) * 2)
    {
        ret = read(fd,
            ((unsigned char *)wood->key) + total,
            sizeof(uint64_t) * 2 - total);
        if (ret <= 0)
        {
            close(fd);
            goto error;
        }
        total += (size_t)ret;
    }
    close(fd);
    return (1);
error:
    wood->error = "failed to generate key.";
    return 0;
}