#include "../../inc/CommandHandler.hpp"

void CommandHandler::HandleQUIT(const std::vector<std::string> &parts, Client & client, Server &server)
{
	client.shouldDisconnect = true;
	Channel *channel = server.getChannelById(client.channelId.value());
	if (channel)
		channel->removeMember(client.id, server);
	client.sendCodeResponse(221, "Goodbye", "QUIT");
}
