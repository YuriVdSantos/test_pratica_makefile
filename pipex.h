/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yvieira- <yvieira-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 12:43:37 by yvieira-          #+#    #+#             */
/*   Updated: 2025/01/12 17:12:26 by yvieira-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
# include "libft/libft.h" 


char **split_first_cmd(char *argv);
char **split_second_cmd(char *argv);
int pipex(int argc, char **argv, char **env);
char **get_path(char **env);
