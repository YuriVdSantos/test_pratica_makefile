/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_pipex.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yurivieiradossantos <yurivieiradossanto    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 19:13:12 by yurivieirad       #+#    #+#             */
/*   Updated: 2025/01/16 22:26:30 by yurivieirad      ###   ########.fr       */
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
    exit(1);
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
    char *default_path = "/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin";
    
    if (!env || !env[0])
        return (ft_split(default_path, ':'));
        
    int i = 0;
    while (env[i] && ft_strncmp(env[i], "126PATH", 4) != 0)
        i++;
    if (!env[i])
        return (ft_split(default_path, ':'));
    return (ft_split(env[i] + 5, ':'));
}

char **split_cmd(char *argv)
{
    char **cmd;
    
    // Se o comando começa com aspas simples ou duplas
    if (argv[0] == '\'' || argv[0] == '"')
    {
        // Remove as aspas e mantém o comando como uma única string
        int len = ft_strlen(argv);
        if (len > 2 && argv[len-1] == argv[0])
        {
            char *temp = ft_substr(argv, 1, len-2);
            cmd = malloc(sizeof(char *) * 2);
            cmd[0] = temp;
            cmd[1] = NULL;
            return cmd;
        }
    }
    
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
        char *prefix = ft_strjoin(all_paths[i], "/");
        temp = ft_strjoin(prefix, cmd);
        free(prefix);
        
        if (access(temp, F_OK | X_OK) == 0)
            return temp;
		else
		{
			free(temp);
			exit(127);
		}
        free(temp);
        i++;
    }
    return NULL;
}

int pipex(char **argv, char **env)
{
    int fd[2];
    char **all_paths = get_path(env);
    char **cmd1_args = split_cmd(argv[2]);
    char **cmd2_args = split_cmd(argv[3]);
    char *cmd1_path = get_command_path(cmd1_args[0], all_paths);
    char *cmd2_path = get_command_path(cmd2_args[0], all_paths);

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

    if (pipe(fd) == -1)
        is_error(1);

    int pid1 = fork();
    if (pid1 < 0)
        is_error(1);

    if (pid1 == 0)
    {
        int infile = open(argv[1], O_RDONLY);
        if (infile == -1)
        {
            perror("Error: opening input file");
            exit(1);
        }
        
        dup2(fd[1], STDOUT_FILENO);
        dup2(infile, STDIN_FILENO);
        close(fd[0]);
        close(fd[1]);
        close(infile);
        
        if (execve(cmd1_path, cmd1_args, env) == -1)
        {
            perror("Error: Command execution failed");
            exit(126);  // Permissão negada ou erro similar
        }
    }

    int pid2 = fork();
    if (pid2 < 0)
        is_error(1);

    if (pid2 == 0)
    {
        int outfile = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (outfile == -1)
            is_error(1);
        dup2(fd[0], STDIN_FILENO);
        dup2(outfile, STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
		if(execve(cmd2_path, cmd2_args, env) == -1)
		{
			write(STDERR_FILENO, "Error: Execution failed.\n", 25);
			exit(127);  // Use 127 para comandos não encontrados
		}
    }

    close(fd[0]);
    close(fd[1]);
    free_array(all_paths);
    free_array(cmd1_args);
    free_array(cmd2_args);
    free(cmd1_path);
    free(cmd2_path);

    int status1, status2;
    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);

    // Use o status do último comando executado
    if (WIFEXITED(status2))
    {
        int exit_status = WEXITSTATUS(status2);
        if (exit_status == 127 || exit_status == 126)
            return exit_status;
        return 0;  // Sucesso se o último comando executou corretamente
    }
    else if (WIFSIGNALED(status2))
        return WTERMSIG(status2) + 128;

    return 1;
}

int main(int argc, char **argv, char **env)
{
    if (argc != 5)
    {
        write(STDERR_FILENO, "Error: wrong prototype: ./pipex file1 cmd1 cmd2 file2\n", 53);
        return 1;
    }
    int infile = open(argv[1], O_RDONLY);
    if (infile == -1)
    {
        perror("Error: opening input file");
        exit(1);
    }
	int outfile = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (outfile == -1)
		is_error(1);

    return pipex(argv, env);
}