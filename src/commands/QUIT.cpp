#include "../../inc/CommandHandler.hpp"

std::string CommandHandler::HandleQUIT(const std::vector<std::string> &parts, Client & client, Server &server)
{
	if (parts.size() > 1)
		return "QUIT :" + parts[1] + "\r\n";
	client.shouldDisconnect = true;
	return "QUIT\r\n";
}
