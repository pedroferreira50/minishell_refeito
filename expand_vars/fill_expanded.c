#include "../minishell.h"

static size_t calc_var_size(const char *arg, t_indices *indices, t_shell *shell)
{
    size_t var_len;

    if (arg[indices->i + 1] == '?')
        return (handle_exit_status(NULL, 0, &indices->i, shell));
    if (isalpha(arg[indices->i + 1]) || arg[indices->i + 1] == '_')
    {
        var_len = get_var_len(arg, indices->i, shell->vars, shell->envp);
        while (isalnum(arg[indices->i + 1]) || arg[indices->i + 1] == '_')
            indices->i++;
        indices->i++;
        return (var_len);
    }
    return (0);
}

static size_t calc_expanded_size(const char *arg, char quote_type, t_shell *shell)
{
    size_t size;
    t_indices indices;

	size = 0;
	indices.i = 0;
	indices.j = 0;
    if (quote_type == '\'')
        return (ft_strlen(arg) + 1);
    while (arg[indices.i])
    {
        if (arg[indices.i] == '$')
        {
            size += calc_var_size(arg, &indices, shell);
            continue;
        }
        size++;
        indices.i++;
    }
    return (size + 1);
}

static void fill_quotes(char *dest, const char *src, t_indices *indices, char quote_type)
{
    if (quote_type == '\'')
    {
        indices->i++;
        while (src[indices->i] && src[indices->i] != '\'')
            dest[indices->j++] = src[indices->i++];
    }
    else if (quote_type == '"')
    {
        indices->i++;
        while (src[indices->i] && src[indices->i] != '"')
            dest[indices->j++] = src[indices->i++];
    }
}

static void fill_exit_status(char *dest, const char *src, t_indices *indices, t_shell *shell)
{
    size_t len;

	len = handle_exit_status(&dest[indices->j], 1, &indices->i, shell);
    indices->j += len;
}

static void fill_var_name(char *dest, const char *src, t_indices *indices, t_shell *shell)
{
    size_t var_len;
    char *key;
    char *val;

	var_len = 0;
    while (ft_isalnum(src[indices->i + 1 + var_len]) || src[indices->i + 1 + var_len] == '_')
        var_len++;
    key = ft_strndup(&src[indices->i + 1], var_len);
    if (!key)
        return;
    val = get_var_value_helper(key, shell->vars, shell->envp);
    if (val)
    {
        ft_strcpy(&dest[indices->j], val);
        indices->j += ft_strlen(val);
    }
    free(key);
    indices->i += var_len + 1;
}

void fill_expanded(char *dest, const char *src, char quote_type, t_shell *shell)
{
    t_indices indices;

	indices.i = 0;
	indices.j = 0;
    if (!dest || !src)
        return;
    if (quote_type == '\'' || quote_type == '"')
        return fill_quotes(dest, src, &indices, quote_type);
    while (src[indices.i])
    {
        if (src[indices.i] == '$' && src[indices.i + 1] == '?')
            fill_exit_status(dest, src, &indices, shell);
        else if (src[indices.i] == '$' && (ft_isalpha(src[indices.i + 1]) || src[indices.i + 1] == '_'))
            fill_var_name(dest, src, &indices, shell);
        else
            dest[indices.j++] = src[indices.i++];
    }
    dest[indices.j] = '\0';
}
