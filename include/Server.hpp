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
#include "Channel.hpp"

class CommandHandler;

extern bool running;

class Server
{
    private:
        uint16_t	        			_port;
        std::string	        			_password;
        int                 			_socket;
        static const int 				MAX_CONNECTIONS = 10;
        std::map<int, Client*>			_clients;
		std::map<std::string, Channel*>	_channels;
        CommandHandler*     		_commandHandler;

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

        void		run();

		// Channel methods
		void		sendMessageToClient(int clientFd, const std::string& message);
		void		removeChannel(const std::string& channelName);
		void		addChannel(const std::string& channelName, Channel* channel);
		Channel*	getChannel(const std::string& channelName);

        // Getters
        uint16_t			getPort() const;
        const std::string&	getPassword() const;
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
