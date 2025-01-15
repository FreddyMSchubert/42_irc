#pragma once

#include <string>
#include <vector>
#include <sstream>
#include "../inc/Server.hpp"

class CommandHandler {
public:
	static std::string HandlePASS(const std::vector<std::string> &parts, Client & client, Server &server);
	static std::string HandleCAP(const std::vector<std::string> &parts, Client & client, Server &server);
	static std::string HandlePING(const std::vector<std::string> &parts, Client & client, Server &server);
	static std::string HandleOPER(const std::vector<std::string> &parts, Client & client, Server &server);
	static std::string HandleNICK(const std::vector<std::string> &parts, Client & client, Server &server);
	static std::string HandleUSER(const std::vector<std::string> &parts, Client & client, Server &server);
	static std::string HandleJOIN(const std::vector<std::string> &parts, Client & client, Server &server);
	static std::string HandleQUIT(const std::vector<std::string> &parts, Client & client, Server &server);
	static std::string HandlePART(const std::vector<std::string> &parts, Client & client, Server &server);
	static std::string HandlePRIVMSG(const std::vector<std::string> &parts, Client & client, Server &server);
	static std::string HandleKICK(const std::vector<std::string> &parts, Client & client, Server &server);
	static std::string HandleINVITE(const std::vector<std::string> &parts, Client & client, Server &server);
	static std::string HandleTOPIC(const std::vector<std::string> &parts, Client & client, Server &server);
	static std::string HandleMODE(const std::vector<std::string> &parts, Client & client, Server &server);
	static std::string HandleDEBUG(const std::vector<std::string> &parts, Client & client, Server &server);

	static std::string HandleCommand(const std::string &inCommand, unsigned int clientId, Server &server);

private:
	// Helper functions
	static std::vector<std::string> split(const std::string &str, char delim);
	static std::string CompleteHandshake(Client & client);
};

typedef std::string (*CommandFunc)(const std::vector<std::string> &, Client &, Server &);
struct CommandMapping
{
	std::string command;
	CommandFunc func;
};
