CC = c++
CFLAGS = -O3 -Wall -Wextra -Werror -std=c++11
INCLUDES = -I./includes
SRCS = ./srcs/main.cpp ./srcs/ExpertSystem.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = expert-system
HEADERS = ./includes/ExpertSystem.hpp

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

all: $(OBJS) $(TARGET)

re: fclean all

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(TARGET)

.PHONY: DEBUG
debug: CFLAGS += -DDEBUG=1
debug: $(OBJS) $(TARGET)
