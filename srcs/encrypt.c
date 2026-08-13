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

// MASTER KEY (Hardcoded - CRITICAL SECURITY WEAKNESS)
static uint64_t    key[2] = {0x1918111009080100, 0x1110980801000908};

// Expanded round keys (generated from master key)
static uint64_t    round_keys[ROUNDS];

// Left rotation: shifts bits left, wraps around to LSB
static uint64_t    rol(uint64_t x, int r)
{
    return (x << r) | (x >> (64 - r));
}

// Right rotation: shifts bits right, wraps around to MSB
static uint64_t    ror(uint64_t x, int r)
{
    return (x >> r) | (x << (64 - r));
}

static void    speck_key_schedule()
{
    static int      flag = 0;	// 🔴 STATIC FLAG - Prevents re-initialization across multiple encrypt calls
    round_keys[0] = key[0];
    uint64_t b = key[1];

    if (flag == 1)
        return ;
    flag = 1;

	// Generate remaining 26 round keys (ROUNDS = 27 total)
    for (int i = 0; i < ROUNDS - 1; i++)
    {
		// Key schedule algorithm (Speck-specific):
        b = (ror(b, 8) + round_keys[i]) ^ i;
        round_keys[i + 1] = rol(round_keys[i], 3) ^ b;
    }
}

static void    speck_encrypt_128(uint64_t* x, uint64_t* y)
{
    for (int i = 0; i < ROUNDS; i++)
    {
		// Round function (Speck ARX cipher - Add, Rotate, XOR):
        *x = (ror(*x, 8) + *y) ^ round_keys[i];
        *y = rol(*y, 3) ^ *x;
    }
}

static void    speck_decrypt_128(uint64_t* x, uint64_t* y)
{
	// Reverse through rounds (26 → 0)
    for (int i = ROUNDS - 1; i >= 0; i--)
    {
        *y = ror(*y ^ *x, 3);
        *x = rol((*x ^ round_keys[i]) - *y, 8);
    }
}

// MULTI-BLOCK ENCRYPTION (MEM BUFFER)
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

// MULTI-BLOCK DECRYPTION (MEM BUFFER)
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
