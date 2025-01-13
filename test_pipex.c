/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_pipex.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yvieira- <yvieira-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 14:35:20 by yvieira-          #+#    #+#             */
/*   Updated: 2025/01/13 19:11:42 by yvieira-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void is_error(int i)
{
    if (i == 0)
        perror("Error: opening input file");
    else if (i == 1)
        perror("Error: duplicating fd or infile");
    else if (i == 2)
        perror("Error: opening output file");
    else if (i == 3)
        perror("Error: duplicating fd or outfile");
    else if (i == 4)
        write(STDERR_FILENO, "Error: Command not found\n", 25);
    else if (i == 5)
        perror("Error: execution failed");
    else if (i == 6)
        write(STDERR_FILENO, "Error: wrong prototype: ./pipex file1 cmd1 cmd2 file2\n", 53);
    exit(EXIT_FAILURE);
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
		is_error(4);
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
		char *prefix = ft_strjoin(all_paths[i], "/");
		temp = ft_strjoin(prefix, cmd);
		free(prefix);

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

	cmd1_path = get_command_path(cmd1_args[0], all_paths);
	cmd2_path = get_command_path(cmd2_args[0], all_paths);
	if (!cmd1_args || !cmd2_args)
	{
		write(STDERR_FILENO, "Error: Command parsing failed.\n", 31);
		free_array(all_paths);
		free_array(cmd1_args);
		free_array(cmd2_args);
		return 1;
	}


	if (!cmd1_path || !cmd2_path)
	{
		free_array(all_paths);
		free_array(cmd1_args);
		free_array(cmd2_args);
		free(cmd1_path);
		free(cmd2_path);
        write(STDERR_FILENO, "Error: Command not found.\n", 26);
        exit(127);
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
		{
			close(fd[0]);
			close(fd[1]);
			close(infile);
			is_error(1);
		}
		close(fd[0]);
		close(fd[1]);
		if(execve(cmd1_path, cmd1_args, env) == -1)
		{
			write(STDERR_FILENO, "Error: Execution failed.\n", 25);
			exit(126);
		}
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

		outfile = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (outfile == -1)
			is_error(2);
		if (dup2(fd[0], STDIN_FILENO) == -1 || dup2(outfile, STDOUT_FILENO) == -1)
		{
			close(fd[0]);
			close(fd[1]);
			close(outfile);
			is_error(3);
		}
		close(fd[0]);
		close(fd[1]);
		if(execve(cmd2_path, cmd2_args, env) == -1)
		{
			perror("Error: Execution failed");
			exit(126);
		}
	}

	close(fd[0]);
	close(fd[1]);
	
    int status1, status2;
    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);

    if (WIFEXITED(status2))
        return WEXITSTATUS(status2);

    return 1;
}

int main(int argc, char **argv, char **env)
{
	if (argc != 5)
		is_error(6);

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
