#pragma once

#include "Socket.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "CommandHandler.hpp"
#include "./Utils/Logger.hpp"

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <array>
#include <chrono>
#include <memory>

extern bool running;

class Server
{
	private:
		int					_port;
		Client				_listening_socket;
		std::vector<Client>	_sockets;

		std::string					_password;
		std::string					_op_password;
		std::vector<Channel>		_channels;
		unsigned int				current_id;

		void updatePoll();
		void acceptNewConnections();
		void handleExistingConnections();

		void HandleClientData(unsigned int clientId);

	public:
		Server(int port, std::string _password, std::string op_password);
		Server(Server const &src) = delete;
		Server &operator=(Server const &src) = delete;
		Server(Server&& other) noexcept = default;
		~Server() = default;

		void Run(); // Main Server loop

		bool isCorrectPassword(std::string passwordAttempt);
		bool isCorrectOperatorPassword(std::string passwordAttempt);

		std::vector<Channel> &getChannels();
		void createChannel(std::string channelName);
		Channel *getChannelById(unsigned int id);
		Channel *getChannelByName(std::string name);

		std::vector<Client> &getClients();
		Client * getClientById(unsigned int id);
		unsigned int getClientIdByName(std::string name);
		Client * getClientByName(std::string name);

		std::string name = "irctic.com";
};
