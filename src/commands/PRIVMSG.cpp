#include "../../inc/CommandHandler.hpp"

#define CTCP_DELIMITER "\x01"

std::string CommandHandler::HandlePRIVMSG(const std::vector<std::string> &parts, Client & client, Server &server)
{
	if (!client.isAuthenticated)
		return ":irctic.com 451 PRIVMSG :You have not registered"; // ERR_NOTREGISTERED
	if (parts.size() < 3)
		return ":irctic.com 412 PRIVMSG :No text to send"; // ERR_NOTEXTTOSEND

	std::string target = parts[1];
	std::string msg = ":" + client.nickname + "!" + client.username + "@irctic.com PRIVMSG " + target + " :";
	for (size_t i = 2; i < parts.size(); i++)
	{
		msg += parts[i];
		if (i < parts.size() - 1)
			msg += " ";
	}
	msg += "\r\n";

	if (msg.size() > 4 && msg.rfind("\x01" "DCC ", 0) == 0) // XXX: File transfer request
	{
		std::string dccCommand = msg.substr(1, msg.size() - 2); // remove \x01's
		std::vector<std::string> dccParts = split(dccCommand, ' ');
		if (dccParts.size() < 3)
			return ":irctic.com 461 DCC :Not enough parameters for DCC"; // ERR_NEEDMOREPARAMS

		std::string dccType = dccParts[1];
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
					return ":irctic.com 401 " + target + " :No such nick/channel"; // ERR_NOSUCHNICK
				targetClientPtr->sendMessage(":" + client.nickname + "!" + client.username + "@irctic.com "
					+ "PRIVMSG " + target + " :" + CTCP_DELIMITER
					+ dccCommand 
					+ CTCP_DELIMITER + "\r\n");
				return ""; // no direct server response to the sender
			}
			else
				return ":irctic.com 400 " + target + " :Cannot DCC SEND to a channel"; // ERR_UNKNOWNERROR
		}
		else if ((dccType == "ACCEPT" || dccType == "REJECT") && dccParts.size() == 4) // DCC ACCEPT/REJECT <filename> <ip> <port> <filesize> -> DCC file accept/decline
		{
			std::cout << "DCC ACCEPT/REJECT request from " << client.getName() << msg << std::endl;
			if (target[0] != '#')
			{
				Client *targetClientPtr = server.getClientByName(target);
				if (!targetClientPtr)
					return ":irctic.com 401 " + target + " :No such nick/channel"; // ERR_NOSUCHNICK
				targetClientPtr->sendMessage(":" + client.nickname + "!" + client.username + "@irctic.com "
					+ "PRIVMSG " + target + " :" + CTCP_DELIMITER
					+ dccCommand 
					+ CTCP_DELIMITER + "\r\n");
				return ""; // no direct server response to the sender
			}
			else
				return ":irctic.com 400 " + target + " :Cannot DCC SEND to a channel"; // ERR_UNKNOWNERROR
		}
		else
			return ":irctic.com 400 DCC :Unsupported DCC command"; // ERR_UNKNOWNERROR
	}

	if (target[0] == '#')
	{
		Channel *channel = server.getChannel(target);
		if (!channel)
			return ":irctic.com 403 " + target + " :No such channel"; // ERR_NOSUCHCHANNEL
		channel->broadcast(msg, server, client.id);
	}
	else if (target != client.nickname)
	{
		std::cout << "Sending message to " << target << std::endl;
		Client *targetClientPtr = server.getClientByName(target);
		if (!targetClientPtr)
			return ":irctic.com 401 " + target + " :No such nick/channel"; // ERR_NOSUCHNICK
		if (!targetClientPtr->isAuthenticated)
			return ":irctic.com 401 " + target + " :No such nick/channel"; // ERR_NOSUCHNICK
		targetClientPtr->sendMessage(msg);
	}

	if (msg.back() == '\n')
		msg.pop_back();

	Logger::Log(LogLevel::INFO, std::string(client.getName() + " sent a message to " + target + ": " + msg));

	return std::string();
}
