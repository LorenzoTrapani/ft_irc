#ifndef MODE_HPP
#define MODE_HPP

#include "ICommand.hpp"

class Mode : public ICommand 
{
    private:
        Server* _server;

    public:
        Mode(Server* server);
        ~Mode();
    
        void execute(Client* client, const std::vector<std::string>& params);
        bool hasPermission(Client* client);
        std::string getCmdName() const;
        void showChannelModes(Client* client, Channel* channel);
		void processModes(Client* client, Channel* channel, const std::vector<std::string>& params);

		//MODE HANDLERS
		void handleOperatorMode(Client* client, Channel* channel, const std::string& nick, bool adding);
		void handleKeyMode(Client* client, Channel* channel, const std::string& key, bool adding);
		void handleInviteOnlyMode(Client* client, Channel* channel, bool adding);
		void handleTopicMode(Client* client, Channel* channel, bool adding);
		void handleChannelLimitMode(Client* client, Channel* channel, int limit, bool adding);
};

#endif