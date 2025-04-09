#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <stdint.h> // Per uint16_t
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <map>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>  // Per inet_addr e inet_ntoa
#include "Logger.hpp"
#include "Utils.hpp"
#include "Client.hpp"

class Server
{
    private:
        uint16_t	        _port;
        std::string	        _password;
        int                 _socket;
        struct sockaddr_in  _serverAddr;
        static const int MAX_CONNECTIONS = 10;

        // Private methods
        void    initSocket();
        void    bindSocket();
        void    listenForConnections();
        void    acceptNewConnection();
        void    initIpAddress();

    public:

        Server(const std::string &port, const std::string &password);
        ~Server();

        void    run();

        // Getters
        const std::string& getHost() const;
        uint16_t getPort() const;
        const std::string& getPassword() const;


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
