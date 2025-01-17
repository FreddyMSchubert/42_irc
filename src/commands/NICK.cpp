#include "../../inc/CommandHandler.hpp"

void CommandHandler::HandleNICK(const std::vector<std::string> &parts, Client & client, Server &server)
{
	if (client.nickname != "")
		return client.sendCodeResponse(400, "Nickname already set", "NICK");
	if (parts.size() != 2)
		return client.sendCodeResponse(461, "Not enough parameters", "NICK");
	if (parts[1][0] == '#')
		return client.sendCodeResponse(432, "Erroneous nickname", parts[1]);

	for (auto &c : server.getClients())
		if (c.nickname == parts[1])
			return client.sendCodeResponse(433, "Nickname is already in use", parts[1]);
	client.nickname = parts[1];
	client.sendCodeResponse(200, "Nickname is now \"" + parts[1] + "\"", "NICK");
	if (client.updateAuthStatus())
		CompleteHandshake(client);
}
