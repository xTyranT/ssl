CC = gcc

CFLAGS = -fsanitize=address -g3 -Wall -Wextra -Werror

INCLUDES = -Iinclude

NAME = ft_ssl

SRCS = $(wildcard src/*.c) $(wildcard src/*/*.c) src/utils/utils.c

OBJS = $(SRCS:.c=.o)

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJS)
	@printf "✓ Linking %s\n" $(NAME)
	@$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	@printf "⠋ Compiling...\r"
	@$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $< > /dev/null 2>&1

clean:
	@rm -f $(OBJS)
	@echo "✓ Cleaned"

fclean: clean
	@rm -f $(NAME)
	@echo "✓ Full cleaned"

re: fclean all