NAME = pipex

INCDIR = include
LIBFTDIR = libft

SRC = test_pipex.c
OBJ = $(SRC:.c=.o)
CC = cc
CFLAGS = -Wall -Wextra -Werror -I$(INCDIR) -I$(LIBFTDIR)

# Biblioteca libft
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
