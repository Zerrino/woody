/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   encrypt.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexafer <alexafer@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/07 14:23:56 by reborn            #+#    #+#             */
/*   Updated: 2026/08/08 15:53:24 by alexafer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody_woodpacker.h"


static uint64_t    key[2] = {0x1918111009080100, 0x1110980801000908};
static uint64_t    round_keys[ROUNDS];

static uint64_t    rol(uint64_t x, int r)
{
    return (x << r) | (x >> (64 - r));
}

static uint64_t    ror(uint64_t x, int r)
{
    return (x >> r) | (x << (64 - r));
}

static void    speck_key_schedule()
{
    static int      flag = 0;
    round_keys[0] = key[0];
    uint64_t b = key[1];

    if (flag == 1)
        return ;
    flag = 1;
    for (int i = 0; i < ROUNDS - 1; i++)
    {
        b = (ror(b, 8) + round_keys[i]) ^ i;
        round_keys[i + 1] = rol(round_keys[i], 3) ^ b;
    }
}

static void    speck_encrypt_128(uint64_t* x, uint64_t* y)
{
    for (int i = 0; i < ROUNDS; i++)
    {
        *x = (ror(*x, 8) + *y) ^ round_keys[i];
        *y = rol(*y, 3) ^ *x;
    }
}

static void    speck_decrypt_128(uint64_t* x, uint64_t* y)
{
    for (int i = ROUNDS - 1; i >= 0; i--)
    {
        *y = ror(*y ^ *x, 3);
        *x = rol((*x ^ round_keys[i]) - *y, 8);
    }
}

void    speack_encrypt(char *mem, size_t len)
{
    size_t i;

    if (len < BLOCK_SIZE)
        return ;
    speck_key_schedule();
    i = 0;
    while (i < len)
    {
        speck_encrypt_128((uint64_t*)&mem[i], (uint64_t*)&mem[i+8]);
        i += BLOCK_SIZE;
    }
}

void    speack_decrypt(char *mem, size_t len)
{
    size_t i;

    if (len < BLOCK_SIZE)
        return ;
    i = 0;
    while (i < len)
    {
        speck_decrypt_128((uint64_t*)&mem[i], (uint64_t*)&mem[i+8]);
        i += BLOCK_SIZE;
    }
}
