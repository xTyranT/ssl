CC = gcc

CFLAGS = -Wall -Wextra -Werror

DEBUG = -g3 -fsanitize=address

INCLUDES = -Iinclude

NAME = ft_ssl

SRC_DIR = src

OBJ_DIR = obj

SRCS = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/*/*.c)

OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

.PHONY: all clean fclean re debug test

all: $(NAME)

$(NAME): $(OBJS)
	@printf "✓ Linking %s\n" $(NAME)
	@$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@printf "Compiling %s\n" $<
	@$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

clean:
	@rm -rf $(OBJ_DIR)
	@echo "✓ Cleaned"

fclean: clean
	@rm -f $(NAME)
	@echo "✓ Full cleaned"

re: fclean all

debug: fclean
	@$(CC) $(CFLAGS) $(DEBUG) $(INCLUDES) $(SRCS) -o $(NAME)

test: fclean $(NAME)
	@echo "Running parser tests..."
	@chmod +x tester.sh
	@./tester.sh || (echo "Tests failed ❌" && exit 1)
	@echo "All tests passed ✅"