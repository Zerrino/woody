/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexafer <alexafer@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/08 09:06:22 by alexafer          #+#    #+#             */
/*   Updated: 2026/08/08 16:35:36 by alexafer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody_woodpacker.h"

int	main(int argc, char **argv)
{
	t_woody	wood;

	ft_bzero(&wood, sizeof(t_woody));
	if (argc != 2)
	{
		wood.error = "you need to specify an program to infect.";
		goto error;
	}

	// Map the original ELF file into memory
	if (map_file(&wood, argv[1]) == 0)
		goto error;
	
	// Parse and extract ELF header (32 or 64 bit based on magic number)
	if (extract_header(&wood) == 0)
		goto error;
	
	// Set up the stub (decompression/decryption code that runs at runtime)
	if (set_stub(&wood) == 0)
		goto error;
	
	// Extract program headers (segments info) from ELF
	if (extract_ph(&wood) == 0)
		goto error;
	
	// Create the packed Woody binary (core packing logic + encryption)
	if (create_woody(&wood) == 0)
		goto error;

	free(wood.stub);
	ft_lstclear(&wood.pt_encrypted, free);
	return (0);
error:
	free(wood.stub);
	ft_lstclear(&wood.pt_encrypted, free);
	ft_putstr_fd("error: ", 2);
	ft_putendl_fd(wood.error, 2);
	return (1);
}
