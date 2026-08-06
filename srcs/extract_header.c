/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   extract_header.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexafer <alexafer@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/02 20:33:04 by alexafer          #+#    #+#             */
/*   Updated: 2026/08/06 23:54:33 by alexafer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody_woodpacker.h"

int	extract_header(t_woody *woody)
{

	woody->header = read_elf(woody, 0x40);
	if (!woody->header || ft_memcmp(woody->header, "\x7F" "ELF", 4))
	{
		woody->error = "file isn't an valid elf file.";
		return (0);
	}
	woody->format = ((char *)woody->header)[EI_CLASS];
	return (1);
}
