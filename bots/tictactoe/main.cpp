#include <exception>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <map>
#include <vector>
#include <string>
#include <iostream>

#include "Bot.hpp"

struct TicTacToeGame {
	bool active = false;
	std::string xPlayer;
	std::string oPlayer;
	char board[9] = {'1','2','3','4','5','6','7','8','9'};
	char currentTurn = 'X';
};
static TicTacToeGame ticTacToeGame;

void onMessage(std::string user, std::string channel, std::string message);
void onDisconnect();
void onConnect();
void onError(std::string message);
void onUserChannelJoin(std::string user, std::string channel);
void onUserChannelLeave(std::string user, std::string channel);

Bot &getBot()
{
	static Bot bot = Bot();

	return bot;
}

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	size_t totalSize = size * nmemb;
	std::string* response = static_cast<std::string*>(userp);
	response->append(static_cast<char*>(contents), totalSize);
	return totalSize;
}

int main(int argc, char *argv[])
{
	if (argc != 6)
	{
		std::cerr << "Usage: <programname> <ip> <port> <nick> <user> <password>" << std::endl;
		return 1;
	}
	try
	{
		Bot &bot = getBot();

		bot.setCallbacks(
				onConnect,
				onError,
				onMessage,
				onDisconnect,
				onUserChannelJoin,
				onUserChannelLeave
			);
		bot.setIp(argv[1]);
		bot.setPort(std::stoi(argv[2]));
		bot.setNick(argv[3]);
		bot.setUser(argv[4]);
		bot.setPassword(argv[5]);

		bot.connectToServer();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}

void onConnect()
{
	std::cout << "Connected to IRC Server!" << std::endl;
	try {
		std::cout << "Trying to athenticate." << std::endl;
		getBot().authenticate();
		getBot().changeChannel("#tictactoe");
		getBot().startPollingForEvents();
	} catch (std::exception& e) {
		std::cerr << "Failed to authenticate: " << e.what() << std::endl;
	}
}

void onError(std::string message)
{
	std::cerr << "ERROR: " << message << std::endl;
}

static void renderBoard(const std::string& channel)
{
	const char *bd = ticTacToeGame.board;
	getBot().sendMessage(channel, std::string(1, bd[0]) + "|" + std::string(1, bd[1]) + "|" + std::string(1, bd[2]));
	getBot().sendMessage(channel, "-----");
	getBot().sendMessage(channel, std::string(1, bd[3]) + "|" + std::string(1, bd[4]) + "|" + std::string(1, bd[5]));
	getBot().sendMessage(channel, "-----");
	getBot().sendMessage(channel, std::string(1, bd[6]) + "|" + std::string(1, bd[7]) + "|" + std::string(1, bd[8]));
	getBot().sendMessage(channel, "Next turn: " + std::string(1, ticTacToeGame.currentTurn));
}

static std::string checkWinOrDraw()
{
	const char (*bd)[9] = &ticTacToeGame.board;
	std::vector<std::vector<int>> wins = {
		{0,1,2}, {3,4,5}, {6,7,8},
		{0,3,6}, {1,4,7}, {2,5,8},
		{0,4,8}, {2,4,6}
	};

	for (auto &w : wins) {
		if ((*bd)[w[0]] == (*bd)[w[1]] && 
			(*bd)[w[1]] == (*bd)[w[2]]) 
		{
			std::string winner(1, (*bd)[w[0]]);
			return "We have a winner: " + winner + " (applause!)\n";
		}
	}

	bool draw = true;
	for (int i = 0; i < 9; ++i) {
		if ((*bd)[i] != 'X' && (*bd)[i] != 'O') {
			draw = false;
			break;
		}
	}
	if (draw)
		return "It's a draw!";

	return "";
}

void onMessage(std::string user, std::string channel, std::string message)
{
	if (channel != "#tictactoe") return;

	Bot &bot = getBot();

	if (message == "!tictactoe")
	{
		if (ticTacToeGame.active)
		{
			bot.sendMessage(channel, "A game is already in progress, dear " + user + 
										". 🏅 Try finishing that before starting anew.");
			return;
		}

		ticTacToeGame = TicTacToeGame(); 
		ticTacToeGame.active = true;
		bot.sendMessage(channel, "Tic-Tac-Toe has begun! Type `!join` to jump in.\n");
		return;
	}

	if (message == "!join")
	{
		if (!ticTacToeGame.active)
		{
			bot.sendMessage(channel, "No game in progress, " + user + 
										". Pray start one with `!tictactoe`.");
			return;
		}

		if (ticTacToeGame.xPlayer.empty())
		{
			ticTacToeGame.xPlayer = user;
			bot.sendMessage(channel, "Splendid, " + user + 
										", thou art 'X'.");
			if (!ticTacToeGame.oPlayer.empty())
			{
				bot.sendMessage(channel, "Both players have joined. Let the game begin!");
				renderBoard(channel);
			}
		}
		else if (ticTacToeGame.oPlayer.empty() && user != ticTacToeGame.xPlayer)
		{
			ticTacToeGame.oPlayer = user;
			bot.sendMessage(channel, "Magnificent, " + user + 
										", thou art 'O'.");
			if (!ticTacToeGame.xPlayer.empty())
			{
				bot.sendMessage(channel, "Both players have joined. Let the game begin!");
				renderBoard(channel);
			}
		}
		else
		{
			bot.sendMessage(channel, "Alas, both roles (X and O) have been assigned. ");
		}
		return;
	}

	if (message.rfind("!move ", 0) == 0)
	{
		if (!ticTacToeGame.active)
		{
			bot.sendMessage(channel, "No game in progress, " + user + 
										". Please start one with `!tictactoe`");
			return;
		}
		if (ticTacToeGame.xPlayer.empty() || ticTacToeGame.oPlayer.empty())
		{
			bot.sendMessage(channel, "We haven't two players yet, dear " + user + ".");
			return;
		}

		bool isXTurn = (ticTacToeGame.currentTurn == 'X');
		std::string currentPlayer = (isXTurn) ? ticTacToeGame.xPlayer 
											: ticTacToeGame.oPlayer;
		if (user != currentPlayer)
		{
			bot.sendMessage(channel, "Please wait for your turn, " + user + ".");
			return;
		}

		int position = -1;
		try
		{
			position = std::stoi(message.substr(6)) - 1; // "!move " is 6 chars
		}
		catch (...)
		{
			bot.sendMessage(channel, "Invalid move command. Use `!move 1` to `!move 9`, e.g. `!move 5`");
			return;
		}

		if (position < 0 || position > 8)
		{
			bot.sendMessage(channel, "Position out of bounds, " + user + 
										"! Choose from 1 to 9.");
			return;
		}

		if (ticTacToeGame.board[position] == 'X' || ticTacToeGame.board[position] == 'O')
		{
			bot.sendMessage(channel, "That space is already occupied, " + user + ".");
			return;
		}

		ticTacToeGame.board[position] = (isXTurn) ? 'X' : 'O';

		bot.sendMessage(channel, "Board updated, " + user + " made a move! \n");

		std::string result = checkWinOrDraw();
		if (!result.empty()) {
			bot.sendMessage(channel, result);
			ticTacToeGame.active = false;
			return;
		}

		ticTacToeGame.currentTurn = (isXTurn) ? 'O' : 'X';
		renderBoard(channel);
		return;
	}

	bot.sendMessage(channel, "Command not found. Try `!tictactoe`, `!join`, or `!move 1` to `!move 9`.");
}

void onDisconnect()
{
	std::cerr << "Disconnected from server!" << std::endl;
}

void onUserChannelJoin(std::string user, std::string channel)
{
	std::cout << "User " << user << " joined channel " << channel << std::endl;

	getBot().directMessage(user, "Welcome " + user + " to the channel! Type !play to begin the game.");
}

void onUserChannelLeave(std::string user, std::string channel)
{
	std::cout << "User " << user << " left channel " << channel << std::endl;

	if (ticTacToeGame.active && (user == ticTacToeGame.xPlayer || user == ticTacToeGame.oPlayer))
	{
		ticTacToeGame.active = false;
		getBot().sendMessage(channel, "Game over! " + user + " has left the game.");
	}

	getBot().directMessage(user, "Goodbye " + user + "! Have a nice day!");
}
