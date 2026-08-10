/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_load.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexafer <alexafer@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/06 15:56:29 by reborn            #+#    #+#             */
/*   Updated: 2026/08/08 16:33:54 by alexafer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody_woodpacker.h"


// ============================================
// ROUND UP TO NEXT PAGE SIZE (4KB)
// Aligns memory addresses to 0x1000 boundaries (ELF page requirement)
// ============================================
static uint64_t rounding_up(uint64_t mem)
{
	// Round up to nearest 4KB boundary (0xfff masks lower 12 bits, +0x1000 adds next page)
    return ((mem & ~0xfff) + 0x1000);
}

// ============================================
// 32-BIT: SETUP PT_NOTE AS EXECUTABLE LOAD SEGMENT
// Repurposes PT_NOTE into PT_LOAD containing the stub
// ============================================
static void    setup_load32(t_woody *wood, Elf32_Phdr *pt_note)
{
	// CHANGE SEGMENT TYPE: PT_NOTE (4) → PT_LOAD (1)
    // This converts metadata segment into executable code segment
    pt_note->p_type = 1;

	// SET PERMISSIONS: Read + Write + Execute (RWX!)
    // Executable+Writable memory is suspicious (malware indicator)
    pt_note->p_flags = PF_X | PF_R | PF_W;

	// File offset where stub will be written (determined by packer)
    pt_note->p_offset = get_writing_point(wood);

	// Physical address (usually equals virtual address for non-PIC)
    pt_note->p_paddr = get_writing_point(wood);

	// Virtual address: align biggest memory region + preserve file alignment offset
    // Calculation: round_up(biggest_mem) + (writing_point & 0xfff)
    // POTENTIAL BUG: Mixing aligned and unaligned offsets could cause overlap
    pt_note->p_vaddr = rounding_up(wood->biggest_mem_used) + (get_writing_point(wood) & 0xfff);

	// Memory size = stub size (code only, no encrypted segments in 32-bit)
    pt_note->p_memsz = wood->stub_size;

	// File size = stub size (same as memsz = no padding)
    pt_note->p_filesz = wood->stub_size;

	// Alignment requirement: 4KB pages
    pt_note->p_align = 0x1000;

	// Store starting memory address for stub runtime calculations
    wood->mem_start = pt_note->p_vaddr;
}

// ============================================
// 64-BIT: SETUP PT_NOTE AS EXECUTABLE LOAD SEGMENT
// Similar to 32-bit but ALLOCATES SPACE FOR ENCRYPTED SEGMENT METADATA
// ============================================
static void    setup_load64(t_woody *wood, Elf64_Phdr *pt_note)
{
    pt_note->p_type = 1;
    pt_note->p_flags = PF_X | PF_R | PF_W;
    pt_note->p_offset = get_writing_point(wood);
    pt_note->p_paddr = get_writing_point(wood);
    pt_note->p_vaddr = rounding_up(wood->biggest_mem_used) + (get_writing_point(wood) & 0xfff);

	// MEMORY SIZE INCLUDES ENCRYPTED SEGMENT TABLE!
    // Formula: stub_size + 8 (counter) + (num_sections × 16 bytes per section)
    // Each section entry: 8 bytes offset + 8 bytes length = 16 bytes
    pt_note->p_memsz = wood->stub_size + 0x8 + (ft_lstsize(wood->pt_encrypted) * 0x10);
    pt_note->p_filesz = wood->stub_size + 0x8 + (ft_lstsize(wood->pt_encrypted) * 0x10);
    pt_note->p_align = 0x1000;

	// Store memory base for stub's runtime calculations
    wood->mem_start = pt_note->p_vaddr;
}

// ============================================
// DISPATCH 32/64-BIT SETUP
// Routes to correct version based on ELF format
// ============================================
void    setup_load(t_woody *wood, void *pt_note)
{
    if (wood->format == ELF32)
        setup_load32(wood, pt_note);
    else
        setup_load64(wood, pt_note);
}
