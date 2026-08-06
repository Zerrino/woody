/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_file.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexafer <alexafer@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/03 00:34:13 by alexafer          #+#    #+#             */
/*   Updated: 2026/08/06 23:55:43 by alexafer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody_woodpacker.h"

int	map_file(t_woody *wood, char *path)
{
	struct stat	buffer;
	int			fd;
	int			fs;

	fd = open(path, O_RDONLY);
	if (fd < 0)
	{
		wood->error = "opening the file failed.";
		goto error;
	}
	fs = fstat(fd, &buffer);
	if (fs < 0)
	{
		wood->error = "fstat failed.";
		goto error;
	}
	if (S_ISDIR(buffer.st_mode))
	{
		wood->error = "you must provide a file.";
		goto error;
	}
	wood->file = mmap(NULL, buffer.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (!wood->file)
	{
		wood->error = "mmap of the file failed.";
		goto error;
	}
	wood->file_len = buffer.st_size;
	wood->file_pos = 0;
	close(fd);
	return (1);
error:
	close(fd);
	return (0);
}
