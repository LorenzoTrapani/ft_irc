NAME = ircserv

CXX = c++
CXXFLAGS = -std=c++98 -Wall -Wextra -Werror -g -I./include

SRC = main.cpp \
		./src/Server.cpp \
		./src/Client.cpp \
		./src/commands/Invite.cpp \
		./src/commands/Join.cpp \
		./src/commands/Kick.cpp \
		./src/commands/Mode.cpp \
		./src/commands/Nick.cpp \
		./src/commands/Pass.cpp \
		./src/commands/Ping.cpp \
		./src/commands/Privmsg.cpp \
		./src/commands/Topic.cpp \
		./src/commands/User.cpp \
		./src/commands/Who.cpp \
		

all: $(NAME)

$(NAME): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "[${NAME}] Compiled successfully."

clean:
	rm -f $(NAME)

fclean: clean
	rm -f $(NAME)

re: fclean all
.PHONY: all clean fclean re
