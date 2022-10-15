NAME = webserv

SRCS = src/main.cpp src/ServerManager.cpp src/Response.cpp src/Client.cpp src/HttpRequest.cpp \
	   src/ConfigFile.cpp src/ConfigParser.cpp src/ServerConfig.cpp src/Location.cpp

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