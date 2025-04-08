#include "Server.hpp"

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        Logger::error("Usage: ./ircserv <port> <password>");
        return 1;
    }

    uint16_t port = static_cast<uint16_t>(std::atoi(argv[1]));
    
    try {
        Server server(port, argv[2]);
        server.run();
    }
    catch (const std::invalid_argument &e) {
		Logger::error("Error: " + std::string(e.what()));
        return 1;
    }
    
    return 0;
}