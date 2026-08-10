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

// ============================================
// SPECK 128/256 CIPHER IMPLEMENTATION
// NSA lightweight block cipher - 128-bit block, 256-bit key
// ============================================

// MASTER KEY (Hardcoded - CRITICAL SECURITY WEAKNESS)
static uint64_t    key[2] = {0x1918111009080100, 0x1110980801000908};
// ^ This is a FIXED KEY for ALL packed binaries! Anyone can decrypt with this.

// Expanded round keys (generated from master key)
static uint64_t    round_keys[ROUNDS];

// ============================================
// BIT ROTATION HELPERS
// Circular left/right shift for 64-bit integers
// ============================================

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

// ============================================
// KEY EXPANSION (KEY SCHEDULE)
// Generates 27 round keys from master key for encryption rounds
// Only runs ONCE due to static flag
// ============================================
static void    speck_key_schedule()
{
    static int      flag = 0;	// 🔴 STATIC FLAG - Prevents re-initialization across multiple encrypt calls
    round_keys[0] = key[0];		// First round key = first half of master key
    uint64_t b = key[1];		// Working register initialized to second half of master key

    if (flag == 1)
        return ;
    flag = 1;

	// Generate remaining 26 round keys (ROUNDS = 27 total)
    for (int i = 0; i < ROUNDS - 1; i++)
    {
		// Key schedule algorithm (Speck-specific):
        // b = rotate_right(b, 8) + round_key[i] ^ i
        b = (ror(b, 8) + round_keys[i]) ^ i;
        round_keys[i + 1] = rol(round_keys[i], 3) ^ b;
    }
}

// ============================================
// SPECK 128-BIT SINGLE BLOCK ENCRYPTION
// Encrypts exactly 16 bytes (two 64-bit words)
// Input: *x (first 8 bytes), *y (second 8 bytes)
// Modifies in-place
// ============================================
static void    speck_encrypt_128(uint64_t* x, uint64_t* y)
{
	// Run 27 rounds of Feistel-like encryption
    for (int i = 0; i < ROUNDS; i++)
    {
		// Round function (Speck ARX cipher - Add, Rotate, XOR):
        // x = rotate_right(x, 8) + y ^ round_key[i]
        *x = (ror(*x, 8) + *y) ^ round_keys[i];

		// y = rotate_left(y, 3) ^ new_x
        *y = rol(*y, 3) ^ *x;
    }
}

// ============================================
// SPECK 128-BIT SINGLE BLOCK DECRYPTION
// Reverses speck_encrypt_128() by running rounds backward
// ============================================
static void    speck_decrypt_128(uint64_t* x, uint64_t* y)
{
	// Reverse through rounds (26 → 0)
    for (int i = ROUNDS - 1; i >= 0; i--)
    {
		// Inverse of y = rol(*y, 3) ^ *x
        // => y = ror(y ^ x, 3)
        *y = ror(*y ^ *x, 3);

		// Inverse of x = ror(*x, 8) + y ^ round_key[i]
        // => x = rol((x ^ round_key[i]) - y, 8)
        *x = rol((*x ^ round_keys[i]) - *y, 8);
    }
}

// ============================================
// MULTI-BLOCK ENCRYPTION (MEM BUFFER)
// Encrypts multiple 16-byte Speck blocks in sequence
// Input: mem = buffer address, len = total bytes
// ============================================
void    speack_encrypt(char *mem, size_t len)
{
    size_t i;

	// Minimum size check: must be at least one block
    if (len < BLOCK_SIZE)
        return ;	// Too small to encrypt - silent failure!

	// Initialize key schedule (runs once per process)
    speck_key_schedule();

	// Process each 16-byte block
    i = 0;
    while (i < len)
    {
		// Encrypt block: mem[i..i+7] = x, mem[i+8..i+15] = y
        speck_encrypt_128((uint64_t*)&mem[i], (uint64_t*)&mem[i+8]);
        i += BLOCK_SIZE;
    }
}

// ============================================
// MULTI-BLOCK DECRYPTION (MEM BUFFER)
// Mirrors speack_encrypt() - decrypts multiple blocks
// No key schedule call (assumes already initialized by encrypt)
// ============================================
void    speack_decrypt(char *mem, size_t len)
{
    size_t i;

    if (len < BLOCK_SIZE)
        return ;	// Too small to decrypt - silent failure!
    i = 0;
    while (i < len)
    {
        speck_decrypt_128((uint64_t*)&mem[i], (uint64_t*)&mem[i+8]);
        i += BLOCK_SIZE;
    }
}
