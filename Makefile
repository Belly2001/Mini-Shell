# Makefile - Mini-Shell
#
# Pour compiler : make
# Pour nettoyer : make clean
# Pour recompiler de zero : make re

# Le compilateur et les flags
CC      = gcc
CFLAGS  = -Wall -Wextra -Werror -g -Iinclude

# Les dossiers
SRC_DIR = src
INC_DIR = include

# Le nom de l'executable
NAME    = minishell

# Les fichiers sources
SRCS    = $(SRC_DIR)/main.c \
          $(SRC_DIR)/parser.c \
          $(SRC_DIR)/builtins.c \
          $(SRC_DIR)/executor.c \
          $(SRC_DIR)/jobs.c \
          $(SRC_DIR)/redirections.c \
          $(SRC_DIR)/pipes.c \
          $(SRC_DIR)/signals.c

# On transforme les .c en .o (les fichiers objets)
OBJS    = $(SRCS:.c=.o)

# Regle par defaut : compiler l'executable
all: $(NAME)

# Linkage : on assemble tous les .o en un executable
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

# Compilation : chaque .c devient un .o
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage : supprime les fichiers objets
clean:
	rm -f $(OBJS)

# Nettoyage complet : supprime aussi l'executable
fclean: clean
	rm -f $(NAME)

# Recompilation complete
re: fclean all

# Ces regles ne sont pas des fichiers
.PHONY: all clean fclean re