/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: reborn <reborn@42belgium.be>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/23 16:06:42 by alexafer          #+#    #+#             */
/*   Updated: 2026/08/06 17:49:09 by reborn           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"


char	*ft_memnmem(const char *haystack, const char *needle, size_t needle_len, size_t len)
{
	size_t	nlen;
	size_t	i;


	i = 0;
	nlen = needle_len;
	if (!nlen)
		return ((char *)haystack);
	while (i < len && len >= nlen)
	{
		if (!ft_memcmp(haystack, needle, nlen))
			return ((char *)haystack);
		haystack++;
		i++;
		len--;
	}
	return (0);
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
