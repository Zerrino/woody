/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: reborn <reborn@42belgium.be>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/08 09:06:22 by alexafer          #+#    #+#             */
/*   Updated: 2026/08/06 17:26:46 by reborn           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody_woodpacker.h"

int	main(int argc, char **argv)
{
	t_woody	wood;

	if (argc != 2)
		goto error;
	ft_bzero(&wood, sizeof(t_woody));
	if (map_file(&wood, argv[1]) == 0)
		goto error;
	if (extract_header(&wood) == 0)
		goto error;
	if (set_stub(&wood) == 0)
		goto error;
	if (extract_ph(&wood) == 0)
		goto error;
	if (create_woody(&wood) == 0)
		goto error;
	free(wood.stub);
	return (0);
error:
	free(wood.stub);
	ft_putendl_fd("error", 2);
	return (1);
}
