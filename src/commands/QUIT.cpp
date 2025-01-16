#include "../../inc/CommandHandler.hpp"

void CommandHandler::HandleQUIT(const std::vector<std::string> &parts, Client & client, Server &server)
{
	(void)server;
	if (parts.size() > 1)
		return client.sendCodeResponse(461, "Not enough parameters", "QUIT");
	client.shouldDisconnect = true;
	client.sendCodeResponse(221, "Goodbye", "QUIT");
}
