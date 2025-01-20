NAME = pipex

LIBFTDIR = libft

SRC = pipex.c utils.c
OBJ = $(SRC:%.c=%.o)
CC = cc 
CFLAGS = -g -Wall -Wextra -Werror -I$(LIBFTDIR)

LIBFT = $(LIBFTDIR)/libft.a

all: $(LIBFT) $(NAME)

$(LIBFT):
	$(MAKE) -C $(LIBFTDIR)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -L$(LIBFTDIR) -lft -o $(NAME)

clean:
	rm -f $(OBJ)
	$(MAKE) -C $(LIBFTDIR) clean

fclean: clean
	rm -f $(NAME)
	$(MAKE) -C $(LIBFTDIR) fclean

re: fclean all