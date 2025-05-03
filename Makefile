NAME = minishell
CC = gcc
CFLAGS = -g -O0 -Wall -Werror -Wextra -I./includes
LIBS = -Llibft -lft -lreadline
RM = rm -rf
OBJDIR = objs

SRCS =  ./builtins/check_execute_builtins.c \
		./builtins/builtins_utils.c \
		./builtins/echo.c \
		./builtins/pwd.c \
		./builtins/cd.c \
		./builtins/export.c \
		./builtins/export_utils.c \
		./builtins/unset.c \
		./builtins/env.c \
		./builtins/exit.c \
		./expand_vars/fill_expanded.c \
		./expand_vars/var_expansion.c \
		./parse_command/parse_command_utils.c \
		./parse_command/parse_command.c \
		./parse_command/process_token.c \
		./pipes_and_execution/execute_commands.c \
		./pipes_and_execution/heredoc.c \
		./pipes_and_execution/pipeline.c \
		./pipes_and_execution/execute_utils.c \
		./pipes_and_execution/execute_utils2.c \
		error.c free_handling.c \
		handle_operator.c loc_vars.c main.c \
		parse_input_utils.c parse_input.c parser.c main_utils.c \
		parsing_utils.c signal.c string_utils.c find_command_path.c

OBJS = $(SRCS:%.c=$(OBJDIR)/%.o)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJS)
	@$(MAKE) --no-print-directory -C libft
	@printf "\033[1;93m[LIBFT]\033[0m"
	@printf "\033[1;92m  Success!\033[0m\n"
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LIBS) -lreadline
	@printf "\033[1;93m[Minishell]\033[0m"
	@printf "\033[1;92m  Success!\033[0m\n"

clean:
	@$(MAKE) clean --no-print-directory -C libft
	@$(RM) $(OBJDIR)
	@printf "\033[1;93m[.o]\033[0m"
	@printf "\033[1;92m  Cleaned!\033[0m\n"

fclean: clean
	@$(MAKE) fclean --no-print-directory -C libft
	@$(RM) $(NAME)
	@printf "\033[1;93m[.a]\033[0m"
	@printf "\033[1;92m  Cleaned!\033[0m\n"

re: fclean all
