#include "../../inc/CommandHandler.hpp"

void CommandHandler::HandleDEBUG(const std::vector<std::string> &parts, Client & client, Server &server)
{
	(void)parts;
	if (!client.isOperator)
		return client.sendCodeResponse(481, "Permission Denied- You're not an IRC operator", "*");
	std::cout << "Channels:" << std::endl;
	for (auto &channel : server.getChannels())
		std::cout << channel.getInfoString() << std::endl;
	std::cout << "Users:" << std::endl;
	for (auto &client : server.getClients())
		std::cout << client.getInfoString() << std::endl;
}
