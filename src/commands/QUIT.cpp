#include "../../inc/CommandHandler.hpp"

void CommandHandler::HandleQUIT(const std::vector<std::string> &parts, Client & client, Server &server)
{
	(void)server;
	if (parts.size() > 1)
		return client.sendCodeResponse(461, "Not enough parameters", "QUIT");
	client.shouldDisconnect = true;
	Channel *channel = server.getChannelById(client.channelId.value());
	if (channel)
		channel->removeMember(client.id, server);
	client.sendCodeResponse(221, "Goodbye", "QUIT");
}
