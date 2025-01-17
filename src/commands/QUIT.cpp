#include "../../inc/CommandHandler.hpp"

void CommandHandler::HandleQUIT(const std::vector<std::string> &parts, Client & client, Server &server)
{
<<<<<<< HEAD
	(void)server;
	(void)parts;
=======
	(void) parts;
>>>>>>> 353aea68b791cf6639b8b8bf5af629c79949d725
	client.shouldDisconnect = true;
	Channel *channel = nullptr;
	if (client.channelId)
		channel = server.getChannelById(client.channelId.value());
	if (channel)
		channel->removeMember(client.id, server);
	client.sendCodeResponse(221, "Goodbye", "QUIT");
}
