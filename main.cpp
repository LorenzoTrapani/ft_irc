#include "Server.hpp"
#include <signal.h>

bool	running = false;

void	handle_sigint(int sig)
{
	if (sig == SIGINT)
	{
		Logger::info("Server stopped");
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
		signal(SIGINT, handle_sigint);
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