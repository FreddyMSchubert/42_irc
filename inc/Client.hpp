#pragma once

#include "Socket.hpp"

#include <sstream>
#include <iomanip>
#include <optional>

class Channel; // avoid circ depend

typedef struct s_socket_state
{
	bool read;			// POLLIN
	bool write;			// POLLOUT
	bool disconnect;	// POLLHUP
	bool error;			// POLLERR
}	t_socket_state;

class Client
{
	public:
		unsigned int				id;
		Socket						socket;
		int							fd;
		t_socket_state				states;
		std::string					inbuffer;
		std::string					outbuffer;
		std::optional<unsigned int>	channelId;

		Client(int fd, Socket socket, int id);	// Client socket constructor
		Client(int port);						// Listening socket constructor
		Client(const Client&) = delete;
		Client& operator=(const Client&) = delete;
		Client(Client&& other) noexcept;
		Client& operator=(Client&& other) noexcept;

		bool isAuthenticated = false;
		bool isOperator = false;
		bool knewPassword = false;
		bool hasReceivedWelcome = false;
		std::string nickname = "";
		std::string username = "";
		bool shouldDisconnect = false;

		std::string getName();
		bool isOperatorIn(Channel *channel);
		void sendMessage(std::string msg);
		void sendCodeResponse(int code, std::string msg, std::string arg = "");
		bool updateAuthStatus();

		std::string getInfoString();
};
