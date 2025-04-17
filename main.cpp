#include "Server.hpp"
#include <signal.h>

bool	running = false;

void	handleSigint(int sig)
{
	if (sig == SIGINT) {
		running = false;
	}
}

int	main(int argc, char **argv)
{
    if (argc != 3)
    {
        Logger::error("Usage: ./ircserv <port> <password>");
        return 1;
    }
    
    try {
        Server server(argv[1], argv[2]);
		signal(SIGINT, handleSigint);
        server.run();
    }
    catch (const std::invalid_argument &e) {
		Logger::error("Parsing error: " + std::string(e.what()));
        return 1;
    }
    catch (const std::exception &e) {
		Logger::error("Server error: " + std::string(e.what()));
        return 1;
    }
    
    return 0;
}

//TODO: rework server.cpp 
//TODO: sistemare messaggi inviati da server a client