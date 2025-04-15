NAME = minishell
CC = gcc
CFLAGS = -g -O0 -Wall -Werror -Wextra
LIBFT = ./libft/libft.a
RM = rm -rf
LIBS = -Llibft -lft -lreadline

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
		env_utils.c error.c free_handling.c \
		handle_operator.c loc_vars.c main.c \
		parse_input_utils.c parse_input.c parser.c \
		parsing_utils.c signal.c string_utils.c find_command_path.c
OBJS = $(SRCS:.c=.o)

all : $(NAME)

$(NAME) : $(OBJS)

	@$(MAKE) --no-print-directory -C libft
	@printf "\033[1;93m[LIBFT]\033[0m"
	@printf "\033[1;92m  Success!\033[0m\n"
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LIBS)
	@printf "\033[1;93m[Minishell]\033[0m"
	@printf "\033[1;92m  Success!\033[0m\n"

%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

clean :
	@$(MAKE) clean --no-print-directory -C ./libft
	@$(RM) $(OBJS)
	@printf "\033[1;93m[.o]\033[0m"
	@printf "\033[1;92m  Cleaned!\033[0m\n"

fclean : clean
	@$(MAKE) fclean --no-print-directory -C ./libft
	@$(RM) $(NAME)
	@printf "\033[1;93m[.a]\033[0m"
	@printf "\033[1;92m  Cleaned!\033[0m\n"

re : fclean all
