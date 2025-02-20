CXX = c++
CXXFLAGS = -std=c++98 -Wall -Wextra -Werror -g
NAME = webserv

SOURCES =	main.cpp
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