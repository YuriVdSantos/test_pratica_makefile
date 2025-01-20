/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yvieira- <yvieira-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 17:48:39 by yvieira-          #+#    #+#             */
/*   Updated: 2025/01/20 17:56:36 by yvieira-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include <unistd.h>
# include <stdlib.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <stdio.h>
# include "libft/libft.h" 

void	free_array(char **cmd);
char	*get_path(char *cmd, char **env);
void	invalid_path(char **cmd);
void	is_error(int i);
char	*build_path(char *dir, char *cmd);
int		check_empty(const char *cmd);
void	child_fork(char **argv, char **env, int *fd);
void	parent_fork(char **argv, char **env, int *fd);


#endif