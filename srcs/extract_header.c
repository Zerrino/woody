/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   extract_header.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexafer <alexafer@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/02 20:33:04 by alexafer          #+#    #+#             */
/*   Updated: 2026/08/03 01:32:54 by alexafer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody_woodpacker.h"

int	extract_header(t_woody *woody)
{

	woody->header = read_elf(woody, 0x40);
	if (!woody->header)
		return (0);
	if (ft_memcmp(woody->header, "\x7F" "ELF", 4))
		return (0);
	woody->format = ((char *)woody->header)[EI_CLASS];
	return (1);
}
