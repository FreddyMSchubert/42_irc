#include "../../inc/CommandHandler.hpp"

void CommandHandler::HandleDCC(std::string target, std::string msg, Client & client, Server &server)
{
	std::cout << "DCC request" << std::endl;
	std::string dccCommand = msg.substr(msg.rfind("\x01" "DCC "), msg.size() - 2); // remove \x01's
	std::vector<std::string> dccParts = split(dccCommand, ' ');
	if (dccParts.size() < 3)
		return client.sendCodeResponse(461, "Not enough parameters", "DCC");

	std::string dccType = dccParts[1];
	std::cout << "DCC parts size: " << dccParts.size() << std::endl;
	std::cout << "DCC type: " << dccType << std::endl;

	if (dccType == "SEND" && dccParts.size() >= 5) // DCC SEND <filename> <ip> <port> <filesize> -> DCC file request
	{
		std::cout << "DCC SEND request from " << client.getName() << msg << std::endl;
		std::string fileName = dccParts[2];
		std::string ipStr    = dccParts[3];
		std::string portStr  = dccParts[4];
		std::string fileSize = (dccParts.size() >= 6) ? dccParts[5] : "Unknown";

		Logger::Log(LogLevel::INFO, "DCC SEND request from " + client.getName() + msg +
									"\n\t=> File: " + fileName +
									",\n\tIP: " + ipStr +
									",\n\tPort: " + portStr +
									",\n\tFileSize: " + fileSize);
		if (target[0] != '#')
		{
			Client *targetClientPtr = server.getClientByName(target);
			if (!targetClientPtr)
				return client.sendCodeResponse(401, "No such nick/channel", target);
			targetClientPtr->sendMessage(":" + client.nickname + "!" + client.username + "@irctic.com "
				+ "PRIVMSG " + target + " :"
				+ dccCommand
				+ "\r\n");
			return ; // no direct server response to the sender
		}
		else
			return client.sendCodeResponse(400, "Cannot DCC SEND to a channel", target);
	}
	else
		client.sendCodeResponse(400, "Unsupported DCC command", "DCC");
}

void CommandHandler::HandlePRIVMSG(const std::vector<std::string> &parts, Client & client, Server &server)
{
	if (!client.isAuthenticated)
		return client.sendCodeResponse(451, "You have not registered", "PRIVMSG");
	if (parts.size() < 3)
		return client.sendCodeResponse(411, "No text to send", "PRIVMSG");

	std::string target = parts[1];
	std::string msg = ":" + client.nickname + "!" + client.username + "@irctic.com PRIVMSG " + target + " ";

	for (size_t i = 2; i < parts.size(); i++)
	{
		msg += parts[i];
		if (i < parts.size() - 1)
			msg += " ";
	}
	msg += "\r\n";

	if (msg.size() > 4 && msg.rfind("\x01" "DCC ") != std::string::npos) // XXX: File transfer request
		return HandleDCC(target, msg, client, server);

	if (target[0] == '#')
	{
		Channel *channel = server.getChannel(target);
		if (!channel)
			return client.sendCodeResponse(403, "No such channel", target);
		channel->broadcast(msg, server, &client);
	}
	else if (target != client.nickname)
	{
		std::cout << "Sending message to " << target << std::endl;
		Client *targetClientPtr = server.getClientByName(target);
		if (!targetClientPtr)
			return client.sendCodeResponse(401, "No such nick/channel", target);
		if (!targetClientPtr->isAuthenticated)
			return client.sendCodeResponse(401, "No such nick/channel", target);
		targetClientPtr->sendMessage(msg);
	}
	else
	{
		client.sendMessage(msg);
	}

	if (msg.back() == '\n')
		msg.pop_back();

	Logger::Log(LogLevel::INFO, std::string(client.getName() + " sent a message to " + target + ": " + msg));

	return ;
}
