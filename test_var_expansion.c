#include "minishell.h"

void test_variable_expansion(void)
{
    t_shell shell;
    char *result;
    
    // Initialize shell with test environment
    char *test_env[] = {"USER=pedro", "HOME=/home/pedro", "PATH=/bin:/usr/bin", NULL};
    init_shell(&shell, test_env);
    
    printf("=== Testing Variable Expansion Issues ===\n\n");
    
    // Test 1: $"HOME"$USER should output HOMEpedro, not $HOMEpedro
    printf("Test 1: $\"HOME\"$USER\n");
    result = expand_variables("$\"HOME\"$USER", '\0', &shell);
    printf("Current result: %s\n", result ? result : "NULL");
    printf("Expected: HOMEpedro\n\n");
    if (result) free(result);
    
    // Test 2: $"HOME" should output HOME, not $HOME
    printf("Test 2: $\"HOME\"\n");
    result = expand_variables("$\"HOME\"", '\0', &shell);
    printf("Current result: %s\n", result ? result : "NULL");
    printf("Expected: HOME\n\n");
    if (result) free(result);
    
    // Test 3: \$USER should output $USER, not \pedro
    printf("Test 3: \\$USER\n");
    result = expand_variables("\\$USER", '\0', &shell);
    printf("Current result: %s\n", result ? result : "NULL");
    printf("Expected: $USER\n\n");
    if (result) free(result);
    
    // Test 4: Multiple backslashes with variables
    printf("Test 4: \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\$USER \\$PATH \\$PWD\n");
    result = expand_variables("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\$USER \\$PATH \\$PWD", '\0', &shell);
    printf("Current result: %s\n", result ? result : "NULL");
    printf("Expected: \\\\\\\\\\\\\\\\$USER $PATH $PWD\n\n");
    if (result) free(result);
    
    free_all_vars(&shell.vars);
}

int main(void)
{
    test_variable_expansion();
    return 0;
}