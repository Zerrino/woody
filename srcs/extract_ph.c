/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   extract_ph.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexafer <alexafer@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/02 22:46:09 by alexafer          #+#    #+#             */
/*   Updated: 2026/08/08 16:33:50 by alexafer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody_woodpacker.h"

// Get number of program headers (e_phnum)
static int	get_ph_number(t_woody *woody)
{
	if (woody->format == ELF32)
		return (woody->header->elf32.e_phnum);
	return (woody->header->elf64.e_phnum);
}

// Get size of each program header entry (e_phentsize)
static int	get_ph_size(t_woody *woody)
{
	if (woody->format == ELF32)
		return (woody->header->elf32.e_phentsize);
	return (woody->header->elf64.e_phentsize);
}

// Get offset to program header table in file (e_phoff)
static int	get_ph_offset(t_woody *woody)
{
	if (woody->format == ELF32)
		return (woody->header->elf32.e_phoff);
	return (woody->header->elf64.e_phoff);
}

// Get highest memory address used by any segment (for allocation sizing)
static uint64_t	get_biggest_mem(t_woody *woody, void *ph)
{
	if (woody->format == ELF32)
		return ((uint64_t)((Elf32_Phdr *)ph)->p_vaddr + ((Elf32_Phdr *)ph)->p_memsz);
	return ((uint64_t)((Elf64_Phdr *)ph)->p_vaddr + ((Elf64_Phdr *)ph)->p_memsz);
}

// Get original entry point (e_entry) from ELF header
uint64_t	get_program_entry(t_woody *woody)
{
	if (woody->format == ELF32)
		return ((uint64_t)woody->header->elf32.e_entry);
	return (woody->header->elf64.e_entry);
}

// ============================================
// ENCRYPT PT_LOAD SEGMENT (64-bit only)
// Creates encrypted segment record and applies Speck cipher
// ============================================
static int	encrypt_pt_load64(t_woody *wood, void *ph)
{
	t_list			*new_lst;
	t_pt_encrypted	*new_pt;

	// Allocate metadata for encrypted segment
	new_pt = (t_pt_encrypted *)malloc(sizeof(t_pt_encrypted));
	if (!new_pt)
	{
		error:
		wood->error = "failed to malloc.";
		return (0);
	}

	// Round file size DOWN to 16-byte boundary (& ~0xf = mask lower 4 bits)
	// Aligns to Speck block size (128 bits = 16 bytes)
	new_pt->size = ((Elf64_Phdr *)ph)->p_filesz & ~0xf;

	// Store file offset where encrypted data lives
	new_pt->file_offset = ((Elf64_Phdr *)ph)->p_offset;

	// Store virtual address (memory location) where this segment maps
	new_pt->memo_offset = ((Elf64_Phdr *)ph)->p_vaddr;

	// Create linked list node
	new_lst = ft_lstnew(new_pt);
	if (!new_lst)
	{
		free(new_pt);
		goto error;
	}

	// Append to encrypted segments list (will be read by stub later)
	ft_lstadd_back(&wood->pt_encrypted, new_lst);

	// APPLY SPECK ENCRYPTION TO FILE DATA
	speack_encrypt((char *)&wood->file[new_pt->file_offset], new_pt->size);
	return (1);
}

// ============================================
// MAIN: EXTRACT PROGRAM HEADERS
// Parses all segments, identifies executable ones, encrypts them
// ============================================nt	extract_ph(t_woody *wood)
{
	int				i;
	int				ph_offset;
	int				ph_number;
	int				ph_size;
	void			*ph;

	// Get program header info from ELF header
	ph_offset = get_ph_offset(wood);
	ph_number = get_ph_number(wood);
	ph_size = get_ph_size(wood);

	// Seek file cursor to program header table start
	if (elf_seek(wood, ph_offset) == -1)
	{
		wood->error = "invalid elf file.";
		return (0);
	}

	// Iterate through all program headers
	i = 0;
	while (i < ph_number)
	{
		// Read next PH from mmap'd file buffer (advances file_pos)
		ph = read_elf(wood, ph_size);
		if (!ph)
		{
			wood->error = "failed to read the program header.";
			return (0);
		}

		// Look for PT_NOTE segment - stores metadata/signature (critical!)
		if (wood->pt_note == 0 && ((wood->format == ELF32 && ((Elf32_Phdr *)ph)->p_type == PT_NOTE) ||
				(wood->pt_note == 0 && wood->format == ELF64 && ((Elf64_Phdr *)ph)->p_type == PT_NOTE)))
			wood->pt_note = ph;		// Store pointer to first PT_NOTE found

		// Track maximum memory footprint (for stub buffer sizing)
		if (wood->biggest_mem_used < get_biggest_mem(wood, ph))
			wood->biggest_mem_used = get_biggest_mem(wood, ph);

		// EXECUTABLE SEGMENTS HANDLING
		// 32-bit: Check PT_LOAD + PF_X (execute flag)
		// BUG: Empty if-block for 32-bit! Nothing happens!
		if (wood->format == ELF32 && ((Elf32_Phdr *)ph)->p_type == PT_LOAD
				&& ((Elf32_Phdr *)ph)->p_flags & PF_X)
		{
			// 🚨 EMPTY - no encryption for 32-bit executable segments!
		}
		// 64-bit: Check PT_LOAD + PF_X
		else if (wood->format == ELF64 && ((Elf64_Phdr *)ph)->p_type == PT_LOAD
				&& ((Elf64_Phdr *)ph)->p_flags & PF_X)
		{
			// Encrypt this segment!
			if (encrypt_pt_load64(wood, ph) == 0)
				return (0);

			// Modify segment flags: add WRITE permission (PT_LOAD becomes RWX?)
			((Elf64_Phdr *)ph)->p_flags |= PF_W;
		}
		i++;
	}

	// 🔴 CRITICAL CHECK: PT_NOTE MUST EXIST
	// If no PT_NOTE segment was found, packing fails completely
	if (wood->pt_note == 0)
	{
		wood->error = "failed to pack.";
		return (0);
	}

	// Set up PT_NOTE segment (injects stub or metadata here)
	setup_load(wood, wood->pt_note);
	return (1);
}
