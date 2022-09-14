NAME = webserv

SRCS = src/main.cpp src/ServerManager.cpp src/Server.cpp src/RequestHandler.cpp src/Client.cpp

HEADERS	= inc/Webserv.hpp

OBJS = $(SRCS:.cpp=.o)

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98
CXXFLAGS = -g3 

RM = rm -rf

RESET = "\033[0m"
BLACK = "\033[1m\033[37m"

all: $(NAME)

$(NAME) : $(OBJS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo $(BLACK)-webserv compiled 🌐 $(RESET)

clean:
	$(RM) $(OBJS)	

fclean: clean
	$(RM) $(NAME)
		@echo OBJ files removed

re: 	fclean all

.PHONY: all clean fclean re