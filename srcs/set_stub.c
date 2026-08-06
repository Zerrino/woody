/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_stub.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: reborn <reborn@42belgium.be>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/06 16:59:28 by reborn            #+#    #+#             */
/*   Updated: 2026/08/06 17:44:59 by reborn           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody_woodpacker.h"


int set_stub(t_woody *wood)
{
    char    *mem_stub;
    char    stub[] =
    {
        0x48, 0x8d, 0x05, 0xf9, 0xff, 0xff, 0xff, 0x48, 0xb9, 0x42, 0x42, 0x42, 
        0x42, 0x42, 0x42, 0x42, 0x42, 0x48, 0x29, 0xc8, 0xff, 0xe0, 0xb8, 0x3c, 
        0x00, 0x00, 0x00, 0xbf, 0x01, 0x00, 0x00, 0x00, 0x0f, 0x05, 
    };

    wood->stub_size = sizeof(stub);
    mem_stub = (char *)malloc(sizeof(stub));
    if (!mem_stub)
        return (0);
    wood->stub = ft_memcpy(mem_stub, stub, sizeof(stub));
    return (1);
}