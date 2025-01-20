/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yvieira- <yvieira-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 16:34:25 by yvieira-          #+#    #+#             */
/*   Updated: 2025/01/20 18:19:20 by yvieira-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	is_error(int i)
{
	if (i == 0)
		perror("\033[31mError: opening input file");
	if (i == 1)
		perror("\033[31mError: duplicating fd[1] or infile");
	if (i == 2)
		perror("\033[31mError: opening output file");
	if (i == 3)
		perror("\033[31mError: duplicating fd[0] or outfile");
	if (i == 4)
		perror("\033[31mError: Command not found");
	if (i == 5)
		perror("\033[31mError: Execution failed");
	if (i == 6)
		perror("\033[31mError: right prototype: ./pipex file1 cmd1 cmd2 file2");
	exit(EXIT_FAILURE);
}

void	invalid_path(char **cmd)
{
	if (cmd && cmd[0])
	{
		ft_putstr_fd("\033[1;31mError: Command not found: ", 2);
		ft_putstr_fd(cmd[0], 2);
		ft_putstr_fd("\033[0m", 2);
		ft_putchar_fd('\n', 2);
	}
	free_array(cmd);
	exit(127);
}

void	free_array(char **cmd)
{
	int	i;

	i = 0;
	if (cmd)
	{
		while (cmd[i])
		{
			free(cmd[i]);
			i++;
		}
		free(cmd);
	}
}

char	*get_path(char *cmd, char **env)
{
	int		i;
	char	*path;
	char	**split_path;

	i = 0;
	if (access(cmd, F_OK | X_OK) == 0)
		return (ft_strdup(cmd));
	while (env[i] && ft_strnstr(env[i], "PATH", 4) == NULL)
		i++;
	if (!env[i])
		return (NULL);
	split_path = ft_split(env[i] + 5, ':');
	if (!split_path)
		return (NULL);
	i = 0;
	while (split_path[i])
	{
		path = build_path(split_path[i], cmd);
		if (access(path, F_OK | X_OK) == 0)
			return (free_array(split_path), path);
		free(path);
		i++;
	}
	free_array(split_path);
	return(NULL);
}

char	*build_path(char *dir, char *cmd)
{
	char	*temp;
	char	*complete_path;

	temp = ft_strjoin(dir, "/");
	if (!temp)
		return (NULL);
	complete_path = ft_strjoin(temp, cmd);
	free(temp);
	return(complete_path);
}