NAME = ircserv

CXX = c++
CXXFLAGS = -std=c++98 -Wall -Wextra -Werror -g -I./include

CYAN = \033[1;36m
RESET = \033[0m

SRC = main.cpp \
		./src/Server.cpp \
		./src/Utils.cpp \
		./src/Logger.cpp \
		./src/Client.cpp \
		./src/Channel.cpp \
		./src/commands/Pass.cpp \
		./src/commands/Nick.cpp \
		./src/CommandHandler.cpp \
		./src/ResponseMessage.cpp \
		./src/commands/User.cpp \
		./src/commands/Ping.cpp \
		./src/commands/Join.cpp \
		./src/commands/PrivMsg.cpp \
		./src/commands/Invite.cpp \
		./src/commands/Topic.cpp \
		./src/commands/Kick.cpp \
		./src/commands/Mode.cpp \
		
		

all: $(NAME)

$(NAME): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "${CYAN}[${NAME}] Compiled successfully.${RESET}"

clean:

fclean: clean
	rm -f $(NAME)

PORT = 6667
PASSW = daje

valgrind: all
	@valgrind --leak-check=full --show-leak-kinds=all --quiet ./$(NAME) "$(PORT)" "$(PASSW)"


re: fclean all
.PHONY: all clean fclean re valgrind
