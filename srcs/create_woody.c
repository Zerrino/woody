/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   create_woody.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: reborn <reborn@42belgium.be>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/06 16:02:09 by reborn            #+#    #+#             */
/*   Updated: 2026/08/06 16:10:29 by reborn           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody_woodpacker.h"

int create_woody(t_woody *wood)
{
    int fd;

    fd = open("./woody", O_CREAT | O_WRONLY, 0755);
    if (fd < 0)
        return (0);
    write(fd, wood->file, wood->file_len);
    return (1);
}