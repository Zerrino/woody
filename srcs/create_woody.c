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

// ============================================
// DETERMINE WRITING POSITION IN FILE
// Where will the stub + encrypted metadata be appended?
// ============================================
uint64_t  get_writing_point(t_woody *wood)
{
	// If section header table exists, overwrite it (reuse space)
    // This allows keeping same file size while replacing metadata
    if (wood->format == ELF32 && wood->file_len > wood->e_shoff && wood->e_shoff != 0)
        return wood->e_shoff;		// Overwrite section header table (32-bit)
    else if (wood->format == ELF64 && wood->file_len > wood->e_shoff && wood->e_shoff != 0)
        return wood->e_shoff;		// Overwrite section header table (64-bit)
    return wood->file_len;			// Otherwise append to end of file
}

// ============================================
// 32-BIT: PATCH STUB AND ELF HEADER
// Updates entry point and patches placeholder with delta
// ============================================
static int  create32(t_woody *wood)
{
    void        *to_change;			// Pointer to 0x42424242 placeholder in stub
    uint32_t    original_entry;
    uint32_t    new_addr;

	// Save original entry point before modifying
    original_entry = wood->header->elf32.e_entry;

	// REDIRECT ELF ENTRY POINT TO STUB
	// Points to PT_LOAD segment with stub
    wood->header->elf32.e_entry = wood->mem_start;

	// INVALIDATE SECTION HEADERS
    // Clears section header info (not needed for execution, hides original structure)
    wood->header->elf32.e_shoff = 0;
    wood->header->elf32.e_shnum = 0;
    wood->header->elf32.e_shstrndx = 0;
    wood->header->elf32.e_shentsize = 0;

	// FIND 0x42424242 PLACEHOLDER IN STUB
    // ft_memnmem searches for pattern within buffer
    to_change = ft_memnmem((void *)wood->stub, "\x42\x42\x42\x42", 4, wood->stub_size);
    if (to_change == 0)
    {
        wood->error = "invalid stub provided.";
        return (0);	// Stub doesn't contain expected placeholder - corruption?
    }

	// Calculate delta (new_entry - original_entry)
    // Stub subtracts this from its address to get real entry point
    new_addr = wood->header->elf32.e_entry - original_entry;

	// Patch placeholder with delta value
    ft_memcpy(to_change, &new_addr, sizeof(new_addr));
    return (1);
}

// ============================================
// 64-BIT: PATCH STUB AND ELF HEADER
// Similar to 32-bit but ALSO encrypts the "WOODY" marker string
// ============================================
static int  create64(t_woody *wood)
{
    void        *to_change;			// Pointer to 0x4242424242424242 placeholder
    void        *to_encrypt;		// Pointer to "WOODY" marker string
    uint64_t    original_entry;
    uint64_t    new_addr;

	// Save original entry point
    original_entry = wood->header->elf64.e_entry;

	// REDIRECT ELF ENTRY POINT TO STUB
    wood->header->elf64.e_entry = wood->mem_start;

	// INVALIDATE SECTION HEADERS
    wood->header->elf64.e_shoff = 0;
    wood->header->elf64.e_shnum = 0;
    wood->header->elf64.e_shstrndx = 0;
    wood->header->elf64.e_shentsize = 0;

	// FIND 8-BYTE PLACEHOLDER IN 64-BIT STUB
    to_change = ft_memnmem((void *)wood->stub, "\x42\x42\x42\x42\x42\x42\x42\x42", 8, wood->stub_size);
    if (to_change == 0)
    {
        wood->error = "invalid stub provided.";
        return (0);
    }

	// Calculate and patch delta (same logic as 32-bit)
    new_addr = wood->header->elf64.e_entry - original_entry;
    ft_memcpy(to_change, &new_addr, sizeof(new_addr));

	// SELF-ENCRYPT THE "WOODY" MARKER STRING
    // Anti-analysis: marker string is encrypted at pack time, decrypted at runtime
    to_encrypt = ft_memnmem((void *)wood->stub, "....WOODY....\n", 14, wood->stub_size);
    if (to_encrypt)
    {
		// Encrypt 16 bytes (Speck block size) - includes padding
        speack_encrypt(to_encrypt, 16);
    }
    return (1);
}

// ============================================
// MAIN: BUILD PACKED BINARY FILE
// Writes modified ELF + stub + encrypted segment metadata
// ============================================
int create_woody(t_woody *wood)
{
    t_list          *lst;		// Iterator for encrypted segments list
    t_pt_encrypted  *pt;		// Current encrypted segment record
    uint64_t        lst_len;	// Number of encrypted segments
    uint64_t        offset;		// Calculated offset for each segment
    int             fd;

	// CREATE OUTPUT FILE "./woody" (HARDCODED NAME!)
    // OVERWRITES EXISTING FILE IF PRESENT
    fd = open("./woody", O_CREAT | O_WRONLY, 0755);
    if (fd < 0)
    {
        wood->error = "failed to open the woody file.";
        return (0);
    }

	// Patch stub and update ELF header based on format
    if (wood->format == ELF32 && create32(wood) == 0)
        return (0);
    else if (wood->format == ELF64 && create64(wood) == 0)
        return (0);
	
	// Write original ELF up to stub insertion point
    // Skips section headers (overwritten by stub+metadata)
    write(fd, wood->file, get_writing_point(wood));

	// Write the decryption stub
    write(fd, wood->stub, wood->stub_size);

	// WRITE ENCRYPTED SEGMENT METADATA TABLE
    // Format: [count][offset1][len1][offset2][len2]...
    lst = wood->pt_encrypted;
    lst_len = ft_lstsize(lst);
    write(fd, &lst_len, 8);

	// Write each encrypted segment record
    while (lst)
    {
        pt = lst->content;

		// BUG: Uses ELF64 e_entry for BOTH 32-bit and 64-bit
        // 32-bit binaries would read garbage here!
        offset = wood->header->elf64.e_entry - pt->memo_offset;
        write(fd, &offset, 8);
        write(fd, &pt->size, 8);
        lst = lst->next;
    }
    close(fd);
    return (1);
}
