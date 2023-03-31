/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masamoil <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/14 10:25:06 by masamoil          #+#    #+#             */
/*   Updated: 2022/09/16 08:36:44 by fleitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft/headers/ft_printf.h"
# include "libft/headers/get_next_line.h"
# include "libft/headers/libft.h"
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <stdio.h>
# include <errno.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <signal.h>
# include <error.h>

# define PROMPT "Minishell> "

typedef enum s_sig_mode
{
	DEFAULT,
	HEREDOC,
	MUTE,
	COMMAND
}	t_sig_mode;

extern int	g_exit_st;

typedef struct s_fd
{
	int	a;
	int	b;
}		t_fd;

typedef union s_pipe_fd
{
	t_fd	frame;
	int		raw[2];
}			t_pipe_fd;

typedef struct s_export
{
	char			*name;
	char			*value;
	struct s_export	*next;
}					t_export;

typedef struct s_token
{	
	char			*value;
	char			**args;
	char			**red;
	int				fds[2];
	struct s_token	*next;
	struct s_token	*prev;
}					t_token;

typedef struct s_data
{
	char			*input;
	char			*trimmed;
	int				i;
	int				fd_in;
	int				fd_out;
	int				*pid;
	t_pipe_fd		*pipe_fd;
	struct s_token	*head;
}					t_data;

typedef struct s_params
{
	char			**env;
	t_export		*export;
	t_data			*data;
	int				old_fd[2];
	int				error;
}					t_params;

//prompt.c
int			prompt(t_data *data, t_params *params);

//signaux.c
void		ft_sig_int(int signal);
void		ft_sig_quit(int signal);
void		ft_sig_heredoc(int signal);
void		ft_signals(t_sig_mode mode);
void		check_exit_status(void);

// BUILT-IN
// cd.c
int			ft_cd(char **arg, t_params *params);
// echo.c
int			ft_echo(char **arg);
// env.c
int			ft_env(char **arg, t_params *params);
int			new_env(char *arg, t_params *params);
// exit.c
int			ft_exit(char **arg, int i, t_params *params, int *old_fd);
// export.c
int			place_new(char *arg, t_export *new, t_export *new2,
				t_params *params);
int			ft_export(char **arg, t_params *params);
// export2.c
t_export	*new_element(char *str);
// print.c
int			print_export(t_params *params);
// pwd.c
int			ft_pwd(char **arg);
// unset.c
int			ft_unset(char **arg, t_params *params);
//EXECUTION
// execution.c
int			ft_execute(t_token *token, t_params *params);
// make_execute_cmd.c
void		make_command(t_token *token, t_params *params, int i, int *old_fd);
// pipes.c
int			ft_pipe(t_token *token, t_params *params, int *pid,
				t_pipe_fd *pipe_fd);
// select_builtin.c
int			get_path(char **arg, t_token *token, t_params *params, int *old_fd);
void		ft_select_builtin(t_token *token, t_params *params, int i,
				int *old_fd);
// INIT
// create_export.c
t_export	*create_export(char **env);
// get_env.c
char		**ft_get_env(char **envp);
// init.c
void		init_data(t_data *data);
void		init_token(t_token *token);
int			initiate_params(t_params *params, char **envp);
int			ft_shlvl(char **envp);
// PARSING
// args_start.c
int			create_arg_tab(t_data *data, t_token *token);
// cut.c
int			ft_cut(t_data *data, t_params *params);
void		replace_quotes(char *str);
// del_quotes.c
int			del_quotes_redir(t_token *token, t_params *params);
// dollar_fct.c
int			if_dollar(char **str_value,
				t_data *data, t_params *par, int *quote);
int			expand(t_token *token, t_data *data, t_params *params);
// only_heredocs
void		only_heredocs(t_data *data);
// parse_redir.c
int			check_redir(t_data *data);
// parse.c
void		jump_quotes(char *str, t_data *data);
int			syntax_check(t_data *data);
int			pipe_cut(t_data *data);
// quotes.c
int			get_next_quote(char quote, t_data *data);
int			check_quotes(t_data *data);
int			move_to_suppress(char *str, int j);
int			del_quotes(t_token *token, t_params *params);
// redirection.c
int			create_red_tab(t_data *data, t_token *token);
// REDIRECTION
// expand_heredoc.c
char		*expand_heredoc(char *line, t_params *params, int *i);				
// here_doc_delim.c
int			delim_quotes(char *delim);
// int			check_delim(char *delim);
// void		check_quote_delim(char *delim, int quotes);
// here_doc.c
int			ft_here_doc(char *str, t_params *params);
// heredoc_line.c
int			get_hd_line(char *del, int fd, int quotes, t_params *params);
// heredoc_utils.c
void		print_error_heredoc(char *str, int fd, t_params *params);
void		free_in_heredoc(t_params *params, int fd);
// redir_fd.c
int			get_fd_input(char *pathname, char *red);
void		ft_redirection(char **str, t_params *params, t_token *token);
// UTILS
// fct_list.c
int			push_back(t_data *data, char *value);
int			ft_size(t_token *lst);
// free.c
void		*free_export(t_export *export);
int			free_exit(t_params *params, t_data *data, int *old_fd);
void		free_struct(t_data *data);
void		free_params(t_params *params);
void		free_command_no(t_params *params, int *old_fd);
// protections.c
int			check_fd(int fd, char *red);
int			check_child(int pid);
int			set_error_malloc(char *str);
void		*error_malloc_return(char *str, void *ptr);
// table_utils.c
void		print_table(char **table);
void		free_table(char **table);
// utils.c
void		replace_quotes2(char *str);
int			ft_space(char c);
void		ft_exit_d(t_data *data, t_params *params);
int			print_error_redir(char *str, char c);

#endif
