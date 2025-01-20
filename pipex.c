/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yvieira- <yvieira-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 16:11:06 by yvieira-          #+#    #+#             */
/*   Updated: 2025/01/20 17:51:10 by yvieira-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int	check_empty(const char *cmd)
{
	return (!cmd || cmd[0] == '\0');
}

void	exec_cmd(char *argv, char **env)
{
	char	**cmd;
	char	*path;

	cmd = ft_split(argv, ' ');
	if (!cmd || !cmd[0])
	{
		free_array(cmd);
		is_error(4);
	}
	path = get_path(cmd[0], env);
	if (!path)
		invalid_path(cmd);
	if (execve(path, cmd, env) == -1)
	{
		free_array(cmd);
		is_error(5);
	}
	free_array(cmd);
}

void	child_fork(char **argv, char **env, int *fd)
{
	int	infile;

	infile = open(argv[1], O_RDONLY, 0644);
	if (infile == -1)
		is_error(0);
	if (dup2(fd[1], STDOUT_FILENO) == -1 || dup2(infile, STDIN_FILENO) == -1)
		is_error(1);
	close(infile);
	close(fd[0]);
	exec_cmd(argv[2], env);
}

void	parent_fork(char **argv, char **env, int *fd)
{
	int	outfile;

	outfile = open(argv[4], O_RDONLY | O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (outfile == -1)
		is_error(2);
	if (dup2(fd[0], STDIN_FILENO) == -1 || dup2(outfile, STDOUT_FILENO) == -1)
		is_error(3);
	close(outfile);
	close(fd[1]);
	exec_cmd(argv[3], env);
}

int	main(int argc, char **argv, char **env)
{
	int		fd[2];
	pid_t	pid;
	int 	status;

	if (argc != 5 || check_empty(argv[2]) || check_empty(argv[4]))
		is_error(6);
	if (pipe(fd) == -1)
		exit(-1);
	pid = fork();
	if (pid == -1)
		exit(-1);
	if (pid == 0)
		child_fork(argv, env, fd);
	else
	{
		waitpid(pid, &status, 1);
 		parent_fork(argv, env, fd);
	}
	return (0);
}