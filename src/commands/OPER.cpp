#include "../../inc/CommandHandler.hpp"

void CommandHandler::HandleOPER(const std::vector<std::string> &parts, Client & client, Server &server)
{
	if (parts.size() != 3)
		return client.sendCodeResponse(461, "Not enough parameters", "OPER");
	if (!client.isAuthenticated)
		return client.sendCodeResponse(451, "You have not registered");

	Client *target = server.getClientByName(parts[1]);
	if (!target)
		return client.sendCodeResponse(401, "No such nick/channel", parts[1]);
	if (server.isCorrectOperatorPassword(parts[2]))
	{
		target->isOperator = true;
		return client.sendCodeResponse(381, "You are now an IRC operator", target->nickname);
	}
	return client.sendCodeResponse(464, "Operator password incorrect");
}
