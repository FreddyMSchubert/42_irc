#include "../../inc/CommandHandler.hpp"

void CommandHandler::HandlePING(const std::vector<std::string> &parts, Client & client, Server &server)
{
	(void)client;
	(void)server;
	if (parts.size() < 2)
		client.sendMessage(":irctic.com PONG irctic.com");
	client.sendMessage(":irctic.com PONG irctic.com :" + parts[1]);
}
