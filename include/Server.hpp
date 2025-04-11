#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <map>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include "Logger.hpp"
#include "Utils.hpp"
#include <algorithm>
#include "Client.hpp"

class CommandHandler;

class Server
{
    private:
        uint16_t	        _port;
        std::string	        _password;
        int                 _socket;
        // struct sockaddr_in  _serverAddr;
        static const int MAX_CONNECTIONS = 10;
        std::map<int, Client*> _clients;
        CommandHandler*     _commandHandler;

        // Private methods
        void    initSocket();
        void    bindSocket();
        void    listenForConnections();
        void    handleConnections();
        void    initIpAddress();
        void    removeClient(int socketFd);
        void    acceptNewConnection();
        bool    handleClientData(int clientFd);
        void    initCommands();
        // void    checkPingClients();
        std::string generatePingToken() const;

    public:
        Server(const std::string &portRaw, const std::string &password);
        ~Server();

        void    run();

        // Getters
        uint16_t getPort() const;
        const std::string& getPassword() const;
        const std::map<int, Client*>& getClients() const;

        // Exceptions
        class ServerException : public std::runtime_error {
            public:
                explicit ServerException(const std::string& msg) : std::runtime_error(msg) {}
        };

        class InvalidArgument : public std::invalid_argument {
            public:
                explicit InvalidArgument(const std::string& msg) : std::invalid_argument(msg) {}
        };
};
#endif
