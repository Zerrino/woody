/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   woody_woodpacker.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexafer <alexafer@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/08 09:06:34 by alexafer          #+#    #+#             */
/*   Updated: 2026/08/03 01:35:06 by alexafer         ###   ########.fr       */
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
	union
	{
		Elf32_Ehdr	elf32;
		Elf64_Ehdr	elf64;
	} *header;



}	t_woody;

int		extract_ph(t_woody *wood);
int		extract_header(t_woody *woody);

int		map_file(t_woody *wood, char *path);

void	*read_elf(t_woody *wood, int size);

#endif
