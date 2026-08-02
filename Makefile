# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: alexafer <alexafer@student.42belgium.be    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/12/06 09:46:53 by zerrino           #+#    #+#              #
#    Updated: 2026/08/03 01:35:20 by alexafer         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = woody_woodpacker

CC = cc
CFLAGS = -Wall -Wextra -Werror


SRC_DIR = srcs
OBJ_DIR = .objs
INC_DIR = include

LIBFT_DIR   := libft
LIBFT       := $(LIBFT_DIR)/libft.a

SRC_FILES = main.c map_file.c read_elf.c extract_header.c extract_ph.c
SRCS = $(addprefix $(SRC_DIR)/, $(SRC_FILES))
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(LIBFT) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -I $(INC_DIR) -c $< -o $@

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR) all

clean:
	$(MAKE) -C $(LIBFT_DIR) clean
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all

.PHONY: all clean fclean re bonus
