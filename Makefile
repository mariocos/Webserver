CXX = c++
CXXFLAGS = -std=c++98 -Wall -Wextra -Werror -g -Wno-c++11-compat
NAME = webserv

SOURCES =	main.cpp utils.cpp checkConnection.cpp\
			parse_request/RequestParse.cpp \
			parse_request/request_gets.cpp \
			signal.cpp Client.cpp Response.cpp create_response.cpp \
			HoleExplorer.cpp Server.cpp loadErrorPages.cpp Routes.cpp printLog.cpp \
			Errors.cpp File.cpp WebSocket.cpp ServerBlock.cpp CgiHandler.cpp \
			Cgi.cpp Post_Dele/Post_exceptions.cpp Post_Dele/post.cpp Post_Dele/del.cpp \
			yaml_parser.cpp yaml_list.cpp yaml_map.cpp yaml_utils.cpp
OBJS_DIR = obj
OBJECTS = $(addprefix $(OBJS_DIR)/, $(SOURCES:%.cpp=%.o))

Color_Off='\033[0m'       # Text Reset
IGreen='\033[0;92m'       # Green
IYellow='\033[0;93m'      # Yellow
ICyan='\033[0;96m'        # Cyan

MSG1 = @echo ${IGreen}"Compiled Successfully ✔︎"${Color_Off}
MSG2 = @echo ${IYellow}"Cleaned Successfully ✔︎"${Color_Off}
MSG3 = @echo ${ICyan}"Cleaned ${NAME} Successfully ✔︎"${Color_Off}

all: $(NAME)

$(NAME): $(OBJECTS)
		$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@
		$(MSG1)

$(OBJS_DIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
		rm -rf $(OBJS_DIR)
		$(MSG2)

fclean: clean
		rm -f $(NAME)
		$(MSG3)

re: fclean all

.PHONY: all clean fclean re