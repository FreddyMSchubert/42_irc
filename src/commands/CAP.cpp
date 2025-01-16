#include "../../inc/CommandHandler.hpp"

void CommandHandler::HandleCAP(const std::vector<std::string> &parts, Client & client, Server &server)
{
	(void)client;
	(void)server;

	if (parts.size() < 2)
		return client.sendCodeResponse(461, "Not enough parameters", "CAP");

	if (parts[1] == "LS")
		return client.sendCodeResponse(302, "", "CAP * LS");
	else if (parts[1] == "REQ")
		return client.sendMessage(":irctic.com CAP * ACK :");
	else if (parts[1] == "END")
		return client.sendMessage(":irctic.com CAP * END :");
	client.sendCodeResponse(502, "CAPABILLITIES command failed", "CAP");
}
