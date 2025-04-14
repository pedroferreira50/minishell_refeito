#include "minishell.h"

char *ft_strndup(const char *src, size_t n)
{
    size_t len;
    char *dest;
    size_t i;

    len = 0;
    i = 0;
    while (len < n && src[len] != '\0')
        len++;
    dest = malloc(len + 1);
    if (!dest)
        return (NULL);
    while (i < len)
    {
        dest[i] = src[i];
        i++;
    }
    dest[len] = '\0';
    return (dest);
}