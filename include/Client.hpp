#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client
{
    private:
	int 		_socketFd;
    std::string _ipAddr;

    public:
	Client();
	~Client();

	void setSocketFd(int socketFd);
	void setIpAddr(std::string ipAddr);

	int getSocketFd();
	std::string getIpAddr();
};

#endif

