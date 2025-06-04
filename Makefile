ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

TARGET			:= libft_malloc_$(HOSTTYPE).so
LINK_TARGET		:= libft_malloc.so
TESTER_TARGET	:= tester_ft_malloc

CC			:= gcc
CFLAGS		:= -Wall -Wextra -Werror -g

SRC_DIR		:= src
SRCS		:= $(patsubst %, $(SRC_DIR)/%, ft_malloc.c ft_realloc.c ft_free.c utils.c)

INC_DIR 	:= include

OBJ_DIR		:= obj
OBJS		:= $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

DEP_DIR		:= dep
DEPS		:= $(SRC:$(SRC_DIR)/%.c=$(DEP_DIR)/%.d)


all: $(TARGET)

$(TARGET):	$(OBJS)
			$(CC) $(CFLAGS) -shared -o $@ $^
			ln -sf $(TARGET) $(LINK_TARGET)

-include $(DEPS)

$(OBJ_DIR)/%.o:$(SRC_DIR)/%.c
			@mkdir -p $(OBJ_DIR) $(DEP_DIR)
			$(CC) $(CFLAGS) -fPIC -o $@ -MMD -MF $(DEP_DIR)/$*.d -I $(INC_DIR) -c $<

$(TESTER_TARGET): main.c $(TARGET)
			$(CC) -Wl,-rpath=. -g -o $(TESTER_TARGET) main.c -L. -lft_malloc -I $(INC_DIR)

test: $(TESTER_TARGET)
			./$(TESTER_TARGET)

clean:
			$(RM) $(OBJS)

fclean:	clean
			$(RM) $(TARGET) $(TESTER_TARGET) $(LINK_TARGET)

re:	fclean
			$(MAKE) all

.PHONY:	all clean fclean re