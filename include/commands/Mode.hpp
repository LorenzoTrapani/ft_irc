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
		void processModes(Client* client, Channel* channel, const std::string& modes);

		//MODE HANDLERS
		void handleOperatorMode(Channel* channel, const std::string& nick, bool adding);
		void handleKeyMode(Channel* channel, const std::string& key, bool adding);
		void handleInviteOnlyMode(Channel* channel, bool adding);
		void handleTopicMode(Channel* channel, bool adding);
		void handleChannelLimitMode(Channel* channel, int limit, bool adding);
};

#endif