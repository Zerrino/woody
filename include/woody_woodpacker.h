/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   woody_woodpacker.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexafer <alexafer@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/08 09:06:34 by alexafer          #+#    #+#             */
/*   Updated: 2026/08/07 22:56:36 by alexafer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WOODY_WOODPACKER_H
# define WOODY_WOODPACKER_H

# include <fcntl.h>
# include <elf.h>
# include <stdio.h>
# include <sys/stat.h>
# include <sys/mman.h>

# include "../libft/libft.h"

typedef enum e_bit
{
	ELF32 = 1,
	ELF64 = 2
}	t_bit;

typedef struct s_woody
{
	uint8_t	*file;
	uint64_t		file_len;
	uint64_t		file_pos;
	int		format;
	void	*pt_note;
	union
	{
		Elf32_Ehdr	elf32;
		Elf64_Ehdr	elf64;
	} *header;

	uint64_t	original_entry;
	uint8_t		*stub;
	uint64_t	stub_size;
	uint64_t	biggest_mem_used;
	uint64_t	mem_start;
	uint64_t	e_shoff;
	char		*error;
}	t_woody;

#define ROUNDS 27
#define BLOCK_SIZE 16

int			set_stub(t_woody *wood);
int			extract_ph(t_woody *wood);
int			create_woody(t_woody *wood);
int			extract_header(t_woody *woody);
int			map_file(t_woody *wood, char *path);
int			elf_seek(t_woody *wood, uint64_t seek_pos);

uint64_t	get_program_entry(t_woody *woody);
uint64_t	get_writing_point(t_woody *wood);

void		*read_elf(t_woody *wood, int size);
void		setup_load(t_woody *wood, void *pt_note);
void		speack_encrypt(char *mem, size_t len);
void		speack_decrypt(char *mem, size_t len);

#endif
