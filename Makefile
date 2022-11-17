TARGET = ft_ping

CC = gcc
CFLAG = #-Wall -Wextra -Werror
RM = rm -rf

INCLUDE = $(wildcard ./include/*.h)
SRC = $(wildcard ./src/*.c)
OBJ = $(SRC:.c=.o)

%.o: %.c
	$(CC) $(CFLAG) -o $@ -I ./include -c $^

all: $(TARGET)

$(TARGET): $(OBJ) $(INCLUDE)
	$(CC) $(CFLAG) -o $@ $(OBJ)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(TARGET)

re: fclean all

.PHONY: all clean fclean re
