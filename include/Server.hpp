#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <stdint.h> // Per uint16_t
#include "Logger.hpp"
#include "Utils.hpp"

class Server
{
    private:
        uint16_t	_port;
        std::string	_password;

    public:
        Server(const std::string &port, const std::string &password);
        ~Server();

        void 		run();
};

#endif
