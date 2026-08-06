/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   woody_woodpacker.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: reborn <reborn@42belgium.be>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/08 09:06:34 by alexafer          #+#    #+#             */
/*   Updated: 2026/08/06 16:23:12 by reborn           ###   ########.fr       */
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
	int		file_len;
	int		file_pos;
	int		format;
	void	*pt_note;
	union
	{
		Elf32_Ehdr	elf32;
		Elf64_Ehdr	elf64;
	} *header;

	uint64_t	biggest_mem_used;
	t_list		*elf_ph;
}	t_woody;

int			extract_ph(t_woody *wood);
int			create_woody(t_woody *wood);
int			extract_header(t_woody *woody);
int			map_file(t_woody *wood, char *path);
int			elf_seek(t_woody *wood, int seek_pos);

uint64_t	get_program_entry(t_woody *woody);

void		*read_elf(t_woody *wood, int size);
void		setup_load(t_woody *wood, void *pt_note);

#endif
