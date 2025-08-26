CXX = c++
CXXFLAGS = -std=c++98 -Wall -Wextra -Werror -g -Wno-c++11-compat
NAME = webserv

SOURCES =	src/main.cpp \
			src/utils/utils.cpp src/utils/signal.cpp src/utils/printLog.cpp \
			src/backend/server/Client.cpp src/backend/server/Errors.cpp src/backend/server/File.cpp src/backend/server/Response.cpp src/backend/server/Routes.cpp \
			src/backend/server/Server.cpp src/backend/server/ServerBlock.cpp src/backend/server/WebSocket.cpp \
			src/backend/connections/checkConnection.cpp src/backend/connections/create_response.cpp src/backend/connections/HoleExplorer.cpp src/backend/connections/loadErrorPages.cpp \
			parse_request/RequestParse.cpp \
			parse_request/request_gets.cpp \
			src/cgi/CgiHandler.cpp src/cgi/Cgi.cpp \
			Post_Dele/Post_exceptions.cpp Post_Dele/post.cpp Post_Dele/del.cpp \
			src/parsing/yaml_parser.cpp src/parsing/yaml_list.cpp src/parsing/yaml_map.cpp src/parsing/yaml_utils.cpp
OBJS_DIR = obj
OBJECTS = $(addprefix $(OBJS_DIR)/, $(SOURCES:%.cpp=%.o))
DIRS = $(sort $(dir $(OBJECTS)))

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

$(OBJECTS): | $(DIRS)

$(DIRS):
	mkdir -p $@

$(OBJS_DIR)/%.o: %.cpp
#	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
		rm -rf $(OBJS_DIR)
		$(MSG2)

fclean: clean
		rm -f $(NAME)
		$(MSG3)

re: fclean all

.PHONY: all clean fclean re