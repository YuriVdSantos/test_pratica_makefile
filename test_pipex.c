/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_pipex.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yurivieiradossantos <yurivieiradossanto    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 14:35:20 by yvieira-          #+#    #+#             */
/*   Updated: 2025/01/12 23:11:29 by yurivieirad      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	is_error(int i)
{
	if (i == 0)
		perror("\033[31mError: opening input file");
	if (i == 1)
		perror("\033[31mError: Duplicating fd[1] or infile");
	if (i == 2)
		perror("\033[31mError: opening output file");
	if (i == 3)
		perror("\033[31mError: Duplicating fd[0] or outfile");
	if (i == 4)
		perror("\033[31mError: Command not found");
	if (i == 5)
		perror("\033[31mError: Execution failed");
	if (i == 6)
		perror("\033[31mError: right prototype: ./pipex file1 cmd1 cmd2 file2");
	exit (EXIT_FAILURE);
}

void free_array(char **arr)
{
	int i = 0;
	if (!arr)
		return;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}

char **get_path(char **env)
{
	int i = 0;
	if (!env)
		return (NULL);
	while (env[i] && ft_strncmp(env[i], "PATH", 4) != 0)
		i++;
	if (!env[i])
		return (NULL);
	return (ft_split(env[i] + 5, ':'));
}

char **split_cmd(char *argv)
{
	char **cmd;
	cmd = ft_split(argv, ' ');
	if (!cmd || !cmd[0])
	{
		free_array(cmd);
		return (NULL);
	}
	return (cmd);
}

char *get_command_path(char *cmd, char **all_paths)
{
	char *temp;
	int i = 0;

	while (all_paths[i])
	{
		temp = malloc(ft_strlen(all_paths[i]) + ft_strlen(cmd) + 2);
		if (!temp)
			return NULL;
		temp = ft_strjoin(ft_strjoin(all_paths[i], "/"), cmd);

		if (access(temp, X_OK) == 0)
			return temp;

		free(temp);
		i++;
	}
	return NULL;
}

int pipex(char **argv, char **env)
{
	int fd[2];
	char **all_paths;
	char **cmd1_args;
	char **cmd2_args;
	char *cmd1_path;
	char *cmd2_path;

	if (pipe(fd) == -1)
	{
		write(STDERR_FILENO, "Error: Pipe creation failed.\n", 29);
		return 1;
	}

	all_paths = get_path(env);
	if (!all_paths)
	{
		write(STDERR_FILENO, "Error: PATH not found.\n", 23);
		return 1;
	}

	cmd1_args = split_cmd(argv[2]);
	cmd2_args = split_cmd(argv[3]);

	if (!cmd1_args || !cmd2_args)
	{
		write(STDERR_FILENO, "Error: Command parsing failed.\n", 31);
		free_array(all_paths);
		free_array(cmd1_args);
		free_array(cmd2_args);
		return 1;
	}

	cmd1_path = get_command_path(cmd1_args[0], all_paths);
	cmd2_path = get_command_path(cmd2_args[0], all_paths);

	if (!cmd1_path || !cmd2_path)
	{
		write(STDERR_FILENO, "Error: Command not found.\n", 26);
		free_array(all_paths);
		free_array(cmd1_args);
		free_array(cmd2_args);
		free(cmd1_path);
		free(cmd2_path);
		return 1;
	}

	int pid1 = fork();
	if (pid1 < 0)
	{
		write(STDERR_FILENO, "Error: Fork failed.\n", 21);
		return 2;
	}

	if (pid1 == 0)
	{
		int	infile;
		
		infile = open(argv[1], O_RDONLY, 0644);
		if (infile == -1)
			is_error(0);
		if (dup2(fd[1], STDOUT_FILENO) == -1 || dup2(infile, STDIN_FILENO) == -1)
			is_error(1);
		close(fd[0]);
		close(fd[1]);
		execve(cmd1_path, cmd1_args, env);
		write(STDERR_FILENO, "Error: Execution failed.\n", 25);
		exit(EXIT_FAILURE);
	}

	int pid2 = fork();
	if (pid2 < 0)
	{
		write(STDERR_FILENO, "Error: Fork failed.\n", 21);
		return 3;
	}

	if (pid2 == 0)
	{
		int	outfile;

		outfile = open(argv[4], O_RDONLY | O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (outfile == -1)
			is_error(2);
		if (dup2(fd[0], STDIN_FILENO) == -1 || dup2(outfile, STDOUT_FILENO) == -1)
			is_error(3);
		close(fd[0]);
		close(fd[1]);
		execve(cmd2_path, cmd2_args, env);
		write(STDERR_FILENO, "Error: Execution failed.\n", 25);
		exit(EXIT_FAILURE);
	}

	close(fd[0]);
	close(fd[1]);
	waitpid(pid1, NULL, 0);
	waitpid(pid2, NULL, 0);

	free_array(all_paths);
	free_array(cmd1_args);
	free_array(cmd2_args);
	free(cmd1_path);
	free(cmd2_path);

	return 0;
}

int main(int argc, char **argv, char **env)
{
	if (argc != 5)
	{
		write(STDERR_FILENO, "Error: Incorrect number of arguments.\n", 39);
		write(STDERR_FILENO, "Usage: ./pipex infile cmd1 cmd2 outfile\n", 40);
		return 1;
	}

	char **all_paths = get_path(env);
	if (!all_paths)
	{
		write(STDERR_FILENO, "Error: PATH not found.\n", 23);
		return 1;
	}

	int result = pipex(argv, env);

	free_array(all_paths);
	return result;
}
