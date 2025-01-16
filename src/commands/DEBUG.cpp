#include "../../inc/CommandHandler.hpp"

std::string CommandHandler::HandleDEBUG(const std::vector<std::string> &parts, Client & client, Server &server)
{
	(void)parts;
	if (!client.isOperator)
		return ":irctic.com 481 * :Permission Denied- You're not an IRC operator"; // ERR_NOPRIVILEGES
	std::cout << "Channels:" << std::endl;
	for (auto &channel : server.getChannels())
		std::cout << channel.getInfoString() << std::endl;
	std::cout << "Users:" << std::endl;
	for (auto &client : server.getClients())
		std::cout << client.getInfoString() << std::endl;
	return "";
}
