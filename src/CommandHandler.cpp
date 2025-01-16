#include "../inc/CommandHandler.hpp"

#include "../inc/Server.hpp" // avoiding circular dependency

std::vector<std::string> CommandHandler::split(const std::string &str, char delim)
{
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string token;
	while (std::getline(ss, token, delim))
		tokens.push_back(token);
	return tokens;
}

std::string CommandHandler::CompleteHandshake(Client &client)
{
	if (client.isAuthenticated && !client.hasReceivedWelcome)
	{
		std::string welcomeMsg = ":irctic.com 001 " + client.nickname + " :Welcome to the IRCtic, " + client.nickname + "!";
		client.sendMessage(welcomeMsg);
		client.hasReceivedWelcome = true;
		Logger::Log(LogLevel::INFO, "Sent RPL_WELCOME to " + client.nickname);
		return welcomeMsg;
	}

	return "";
}

std::string CommandHandler::HandleCommand(const std::string &inCommand, unsigned int clientId, Server & server)
{
	Client *clientPtr = server.getClientById(clientId);
	if (!clientPtr)
		return ":irctic.com 401 * :No such nick/channel"; // ERR_NOSUCHNICK
	Client &client = *clientPtr;

	Logger::Log(LogLevel::INFO, "Received command from " + client.getName() + ": " + inCommand);

	std::vector<std::string> parts = split(inCommand, ' ');
	if (parts.empty())
		return ":irctic.com 421 :Unknown command"; // ERR_UNKNOWNCOMMAND

	static const std::vector<CommandMapping> commandMappings = {
		{ "PASS", &CommandHandler::HandlePASS },
		{ "CAP", &CommandHandler::HandleCAP },
		{ "PING", &CommandHandler::HandlePING },
		{ "OPER", &CommandHandler::HandleOPER },
		{ "NICK", &CommandHandler::HandleNICK },
		{ "USER", &CommandHandler::HandleUSER },
		{ "JOIN", &CommandHandler::HandleJOIN },
		{ "QUIT", &CommandHandler::HandleQUIT },
		{ "PART", &CommandHandler::HandlePART },
		{ "PRIVMSG", &CommandHandler::HandlePRIVMSG },
		{ "KICK", &CommandHandler::HandleKICK },
		{ "INVITE", &CommandHandler::HandleINVITE },
		{ "TOPIC", &CommandHandler::HandleTOPIC },
		{ "MODE", &CommandHandler::HandleMODE },
		{ "DEBUG", &CommandHandler::HandleDEBUG }
	};

	for (const auto &mapping : commandMappings)
		if (parts[0] == mapping.command)
			return mapping.func(parts, client, server);

	return ":irctic.com 421 " + parts[0] + " :Unknown command"; // ERR_UNKNOWNCOMMAND
}
