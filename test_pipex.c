/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_pipex.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yvieira- <yvieira-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 14:35:20 by yvieira-          #+#    #+#             */
/*   Updated: 2025/01/12 17:42:32 by yvieira-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	free_array(char **arr)
{
	int	i = 0;
	while (arr[i])
	{
		free(arr);
		i++;
	}
	free(arr);
}

char **get_path(char **env)
{
	int i = 0;
	if(!env)
		return (NULL);
	
	while (env[i] && ft_strncmp(env[i], "PATH", 4) != 0)
		i++;
	if (!env[i])
		return (NULL);
	return (ft_split(env[i] + 5, ':'));
}

char **split_first_cmd(char *argv)
{
	char **cmd;
	cmd = ft_split(argv, ' ');
	if(!cmd || !cmd[0])
	{
		free(cmd);
		return (NULL);
	}
	printf("%s\n", cmd[0]);
	printf("%s\n", cmd[1]);
	return (cmd);
}

char **split_second_cmd(char *argv)
{
	char **cmd;
	cmd = ft_split(argv, ' ');
	if(!cmd || !cmd[0])
	{
		free(cmd);
		return (NULL);
	}
	printf("%s\n", cmd[0]);
	printf("%s\n", cmd[1]);
	return (cmd);
}

int main(int argc, char **argv, char **env)
{
	if(argc!=5)
		return -1;
	char **all_paths = get_path(env);
	int i = 0;
	while(all_paths[i])
	{
		printf("%s\n", all_paths[i]);
		i++;
	}
	char **first_cmd = split_first_cmd(argv[2]);
	int	j = 0;
	while(first_cmd[j])
	{
		printf("%s\n", first_cmd[j]);
		j++;
	}

	free_array(all_paths);
	free_array(first_cmd);

	return 0;
}