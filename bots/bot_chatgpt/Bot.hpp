#pragma once

#include <iostream>
#include <queue>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <poll.h>
#include <cstring>
#include <algorithm>
#include <exception>
#include <string>
#include <functional>
#include <string>
#include <curl/curl.h>

class Bot;

typedef std::function<void(std::string, std::string, std::string)> onMessageCallback; //channel, user, message, botRef
typedef std::function<void(void)> onDisconnectCallback;
typedef std::function<void(void)> onConnectCallback;
typedef std::function<void(std::string)> onErrorCallback;
typedef std::function<void(std::string, std::string)> onUserChannelJoinCallback;
typedef std::function<void(std::string, std::string)> onUserChannelLeaveCallback;

class Socket
{
	private:
		onDisconnectCallback		_onDisconnectCallback = nullptr;
		onMessageCallback			_onMessageCallback = nullptr;
		onConnectCallback			_onConnectCallback = nullptr;
		onErrorCallback				_onErrorCallback = nullptr;
		onUserChannelJoinCallback	_onUserChannelJoinCallback = nullptr;
		onUserChannelLeaveCallback	_onUserChannelLeaveCallback = nullptr;

		std::string					_socket_ip;
		int							_socket_port;
		std::queue<std::string>		_messages;

		int _socket_fd = -1;
		struct sockaddr_in _socket;
		static volatile bool _running;
		bool error = false;
		bool _passwordCorrect = false;
		bool _authenticated = false;

		void _setNonBlocking();
		void _sendMessage(std::string msg);
		void _parseBuffer(std::string buffer);

		static void signalHandler(int signum);

	public:
		Socket();
		~Socket();

		void connectToServer(std::string ip, int port);

		void setOnMessageCallback(onMessageCallback callback) { _onMessageCallback = callback; };
		void setOnDisconnectCallback(onDisconnectCallback callback) { _onDisconnectCallback = callback; };
		void setOnConnectCallback(onConnectCallback callback) { _onConnectCallback = callback; };
		void setOnErrorCallback(onErrorCallback callback) { _onErrorCallback = callback; };
		void setOnUserChannelJoinCallback(onUserChannelJoinCallback callback) { _onUserChannelJoinCallback = callback; };
		void setOnUserChannelLeaveCallback(onUserChannelJoinCallback callback) { _onUserChannelLeaveCallback = callback; };

		void queueMessage(std::string msg);

		void Run();

};

class Bot
{
	private:
		std::string	_ip = "127.0.0.1";
		int			_port = 6667;
		std::string _password = "password";
		std::string	_current_channel;

		//bot
		std::string	_nick = "bot";
		std::string	_user = "bot";

		//socket
		Socket socket;
	public:
		Bot();
		~Bot();

		Bot(const Bot &bot) = delete;
		Bot &operator=(const Bot &bot) = delete;

		void setIp(std::string ip) { _ip = ip; }
		void setPort(int port) { _port = port; }
		void setPassword(std::string password) { _password = password; }
		void setNick(std::string nick) { _nick = nick; }
		void setUser(std::string user) { _user = user; }

		std::string getIp() {return _ip;}
		int			getPort() {return _port;}
		std::string	getPassword() {return _password;}
		std::string	getNick() {return _nick;}
		std::string getUser() {return _user;}

		void connectToServer();
		void sendRawMessage(std::string msg);
		void directMessage(std::string user, std::string msg);
		void sendMessage(std::string msg);
		void sendMessage(std::string channelname, std::string msg);
		void changeChannel(std::string channelname);
		void changeChannel(std::string channelname, std::string password);
		void authenticate();
		void setCallbacks(onConnectCallback onConnect,
					onErrorCallback onError,
					onMessageCallback onMessage,
					onDisconnectCallback onDisconnect,
					onUserChannelJoinCallback onUserChannelJoin,
					onUserChannelLeaveCallback onUserChannelLeave);

		void startPollingForEvents();

		std::string ApiCall(const std::string &prompt);
};
