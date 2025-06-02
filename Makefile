ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME		:= libft_malloc_$(HOSTTYPE).so
TESTER_NAME	:= tester_ft_malloc

CC			:= gcc
CFLAGS		:= -Wall -Wextra -Werror -g

SRC_DIR		:= src
SRCS		:= $(patsubst %, $(SRC_DIR)/%, ft_malloc.c ft_realloc.c ft_free.c utils.c)

INC_DIR 	:= include

OBJ_DIR		:= obj
OBJS		:= $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

DEP_DIR		:= dep
DEP			:= $(SRC:$(SRC_DIR)/%.c=$(DEP_DIR)/%.d)


all: $(NAME)

$(NAME):	$(OBJS)
			$(CC) $(CFLAGS) -shared -o $(NAME) $(OBJS)
			ln -sf $(NAME) libft_malloc.so

-include $(DEP)

$(OBJ_DIR)/%.o:$(SRC_DIR)/%.c
			@mkdir -p $(OBJ_DIR) $(DEP_DIR)
			$(CC) $(CFLAGS) -fPIC -o $@ -MMD -MF $(DEP_DIR)/$*.d -I $(INC_DIR) -c $<

$(TESTER_NAME): main.c
#			$(CC) -g -o $(TESTER_NAME) main.c -L. -lft_malloc -I $(INC_DIR)
			$(CC) -Wl,-rpath=/home/sgaubert/Documents/ft_malloc -g -o $(TESTER_NAME) main.c -L. -lft_malloc -I $(INC_DIR)

test: $(NAME) $(TESTER_NAME)
			./$(TESTER_NAME)

clean:
			$(RM) $(OBJS)

fclean:	clean
			$(RM) $(NAME) $(TESTER_NAME)
			$(RM) libft_malloc.so

re:	fclean
			$(MAKE) all

.PHONY:	all clean fclean re