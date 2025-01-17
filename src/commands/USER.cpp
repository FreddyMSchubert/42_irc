#include "../../inc/CommandHandler.hpp"

void CommandHandler::HandleUSER(const std::vector<std::string> &parts, Client & client, Server &server)
{
	if (client.username != "")
		return client.sendCodeResponse(400, "Username already set");
	if (parts.size() < 2)
		return client.sendCodeResponse(461, "Not enough parameters", "USER");
	if (parts[1].size() <= 0 || parts[1][0] == '#')
		return client.sendCodeResponse(432, "Erroneous username", parts[1]);

	for (auto &c : server.getClients())
		if (c.username == parts[1])
			return client.sendCodeResponse(464, "Username is already in use");
	client.username = parts[1];
	client.sendCodeResponse(200, "Username set to " + parts[1]);
	if (client.updateAuthStatus())
		CompleteHandshake(client);
}
