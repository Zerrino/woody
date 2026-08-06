/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_stub.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexafer <alexafer@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/06 16:59:28 by reborn            #+#    #+#             */
/*   Updated: 2026/08/06 23:52:08 by alexafer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody_woodpacker.h"

static int stub32(t_woody *wood)
{
    char    *mem_stub;
    char    stub[] =
    {
        0x53, 0x51, 0x52, 0xd9, 0xee, 0xd9, 0x74, 0x24, 0xf4, 0xeb, 0x0e, 0x2e,
        0x2e, 0x2e, 0x2e, 0x57, 0x4f, 0x4f, 0x44, 0x59, 0x2e, 0x2e, 0x2e, 0x2e,
        0x0a, 0x59, 0x51, 0x83, 0xc1, 0x08, 0xb8, 0x04, 0x00, 0x00, 0x00, 0xbb,
        0x01, 0x00, 0x00, 0x00, 0xba, 0x0e, 0x00, 0x00, 0x00, 0xcd, 0x80, 0x58,
        0x2d, 0x42, 0x42, 0x42, 0x42, 0x83, 0xe8, 0x03, 0x5a, 0x59, 0x5b, 0xff,
        0xe0,
    };
    wood->stub_size = sizeof(stub);
    mem_stub = (char *)malloc(sizeof(stub));
    if (!mem_stub)
    {
        wood->error = "failed to malloc the stub.";
        return (0);
    }
    wood->stub = ft_memcpy(mem_stub, stub, sizeof(stub));
    return (1);
}

static int stub64(t_woody *wood)
{
    char    *mem_stub;
    char    stub[] =
    {
        0xeb, 0x0e, 0x2e, 0x2e, 0x2e, 0x2e, 0x57, 0x4f, 0x4f, 0x44, 0x59, 0x2e,
        0x2e, 0x2e, 0x2e, 0x0a, 0x57, 0x56, 0x52, 0xb8, 0x01, 0x00, 0x00, 0x00,
        0xbf, 0x01, 0x00, 0x00, 0x00, 0x48, 0x8d, 0x35, 0xde, 0xff, 0xff, 0xff,
        0xba, 0x0e, 0x00, 0x00, 0x00, 0x0f, 0x05, 0x48, 0x8d, 0x05, 0xce, 0xff,
        0xff, 0xff, 0x48, 0xbf, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42,
        0x48, 0x29, 0xf8, 0x5a, 0x5e, 0x5f, 0xff, 0xe0,
    };
    wood->stub_size = sizeof(stub);
    mem_stub = (char *)malloc(sizeof(stub));
    if (!mem_stub)
    {
        wood->error = "failed to malloc the stub.";
        return (0);
    }
    wood->stub = ft_memcpy(mem_stub, stub, sizeof(stub));
    return (1);
}

int set_stub(t_woody *wood)
{
    if (wood->format == ELF32)
        return (stub32(wood));
    return (stub64(wood));
}
