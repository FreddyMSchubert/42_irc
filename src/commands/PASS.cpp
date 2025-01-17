#include "../../inc/CommandHandler.hpp"

void CommandHandler::HandlePASS(const std::vector<std::string> &parts, Client & client, Server &server)
{
	if (parts.size() != 2)
		return client.sendCodeResponse(461, "Not enough parameters"); // ERR_NEEDMOREPARAMS
	if (client.knewPassword)
		return client.sendCodeResponse(462, "You already inputted the password correctly"); // ERR_ALREADYREGISTRED

	if (server.isCorrectPassword(parts[1]))
	{
		client.knewPassword = true;
		client.sendCodeResponse(900, "Password correct"); // Custom success reply
		if (client.updateAuthStatus())
			CompleteHandshake(client);
		return;
	}
	client.sendCodeResponse(464, "Password incorrect"); // ERR_PASSWDMISMATCH
}
