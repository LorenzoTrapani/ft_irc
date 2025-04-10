#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>

class Client {
private:
    int _socketFd;
    std::string _ipAddr;
    std::string _nickname;
    std::string _username;
    std::string _inputBuffer; // Buffer per i dati in ingresso
    bool _authenticated;

public:
    Client();
    ~Client();

    // Getters
    int getSocketFd() const;
    const std::string& getIpAddr() const;
    const std::string& getNickname() const;
    const std::string& getUsername() const;
    bool isAuthenticated() const;

    // Setters
    void setSocketFd(int socketFd);
    void setIpAddr(const std::string& ipAddr);
    void setNickname(const std::string& nickname);
    void setUsername(const std::string& username);
    void setAuthenticated(bool authenticated);

    // Buffer management
    void appendToBuffer(const std::string& data);
    std::vector<std::string> extractCommands();
};

#endif
