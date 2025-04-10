#include "Server.hpp"

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        Logger::error("Usage: ./ircserv <port> <password>");
        return 1;
    }
    
    try {
        Server server(argv[1], argv[2]);
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