#include "../inc/Utils/Logger.hpp"
#include "../inc/Server.hpp"

#include <string>
#include <csignal>
#include <atomic>
#include <unistd.h>

volatile sig_atomic_t running = true;

void handle_sigint(int signal)
{
	if (signal != SIGINT)
		return ;
	write(STDOUT_FILENO, "SIGINT received, shutting down server...\n", 40);
	running = false;
}

int main(int argc, char **argv)
{
	if (argc != 4)

	{
		Logger::Log(LogLevel::ERROR, std::string("Incorrect arguments. Usage: ") + argv[0] + " <port> <password> <operator password>");
		return -1;
	}

	std::signal(SIGINT, handle_sigint);

	int port = -1;
	std::string password = argv[2];
	std::string opPassword = argv[3];
	try
	{
		port = std::stoi(argv[1]);
	}
	catch (const std::exception &e)
	{
		Logger::Log(LogLevel::ERROR, std::string("Problem parsing arguments: ") + e.what());
		return -1;
	}
	if (port <= 0 || port > 65535)
	{
		Logger::Log(LogLevel::ERROR, "Trouble parsing port");
		return -1;
	}

	try
	{
		Server cm(port, password, opPassword);
		cm.Run();
	}
	catch (const std::exception &e)
	{
		Logger::Log(LogLevel::ERROR, std::string("Failed to create server: ") + e.what());
		return -1;
	}

	return 0;
}
