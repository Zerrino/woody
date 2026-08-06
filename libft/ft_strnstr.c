/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexafer <alexafer@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/23 16:06:42 by alexafer          #+#    #+#             */
/*   Updated: 2026/08/06 21:52:42 by alexafer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char *ft_memnmem(const char *haystack, const char *needle,
	size_t needle_len, size_t len)
{
	size_t i;

	if (needle_len == 0)
		return ((char *)haystack);
	if (len < needle_len)
		return (NULL);

	i = 0;
	while (i <= len - needle_len)
	{
		if (ft_memcmp(haystack + i, needle, needle_len) == 0)
			return ((char *)(haystack + i));
		i++;
	}
	return (NULL);
}

char	*ft_strnstr(const char *haystack, const char *needle, size_t len)
{
	size_t	nlen;

	nlen = (size_t)ft_strlen((char *)needle);
	if (!nlen)
		return ((char *)haystack);
	while (*haystack && len >= nlen)
	{
		if (!ft_strncmp(haystack, needle, nlen))
			return ((char *)haystack);
		haystack++;
		len--;
	}
	return (0);
}
