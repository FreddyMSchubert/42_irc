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

static const char* TICTACTOE_HELP_JOKE = 
	"I say, Mr. Elsher 🕵️‍♂️, if you can't handle a mere 3 dm x 3 dm board, "
	"perhaps you'd prefer a regal game of chess? (¬‿¬) 🤘💀💭🖕🍆🤏";

// Minimal struct to hold game state
struct TicTacToeGame {
	bool active = false;
	std::string xPlayer;  // Nick of Player X
	std::string oPlayer;  // Nick of Player O
	char board[9] = {'1','2','3','4','5','6','7','8','9'};
	char currentTurn = 'X'; // 'X' or 'O'
};

// We’ll store a single game for demonstration.
// If you want multiple channels playing, store a map<channel, TicTacToeGame>.
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

std::string Bot::ApiCall(const std::string &prompt)
{
	CURL* curl = curl_easy_init();
	if (!curl)
		return "Error: unable to init cURL";

	std::string response;
	curl_global_init(CURL_GLOBAL_DEFAULT);
	struct curl_slist* headers = NULL;

	curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "Authorization: Bearer key");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	std::string jsonBody =
		"{"
			"\"model\":\"gpt-3.5-turbo\","
			"\"messages\":[{\"role\":\"user\",\"content\":\"" + prompt + "\"}],"
			"\"max_tokens\":150"
		"}";

	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonBody.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK)
		return "cURL request failed: " + std::string(curl_easy_strerror(res));

	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);
	curl_global_cleanup();

	if (response.empty())
		return "Error: Empty response from API";

	size_t errorPos = response.find("\"error\":");
	if (errorPos != std::string::npos)
	{
		size_t msgStart = response.find("\"message\":\"", errorPos);
		if (msgStart != std::string::npos)
		{
			msgStart += 11;
			size_t msgEnd = response.find("\"", msgStart);
			if (msgEnd != std::string::npos)
				return "API Error: " + response.substr(msgStart, msgEnd - msgStart);
		}
		return "API Error: Unknown error format in response";
	}

	size_t contentPos = response.find("\"content\": \"");
	if (contentPos == std::string::npos)
		return "Error: Could not find content in message";

	contentPos += 12;
	size_t endPos = response.find("\"", contentPos);
	if (endPos == std::string::npos)
		return "Error: Malformed response - missing closing quote";

	std::string content = response.substr(contentPos, endPos - contentPos);

	size_t pos = 0;
	while ((pos = content.find("\\n", pos)) != std::string::npos)
	{
		content.replace(pos, 2, "\n");
		pos += 1;
	}

	return content;
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
		std::cout << "penis" << std::endl;
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

// custom function to handle connection
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

// custom function to handle errors
void onError(std::string message)
{
	std::cerr << "ERROR: " << message << std::endl;
}

// Utility function: Render the board
static std::string renderBoard() {
	// e.g. "1|X|O\n---\n4|X|5\n---\nO|X|9"
	std::string b;
	char *bd = ticTacToeGame.board;
	b += bd[0]; b += '|'; b += bd[1]; b += '|'; b += bd[2]; b += "\n---\n";
	b += bd[3]; b += '|'; b += bd[4]; b += '|'; b += bd[5]; b += "\n---\n";
	b += bd[6]; b += '|'; b += bd[7]; b += '|'; b += bd[8];
	return b;
}

// Utility to check for a winner or draw
static std::string checkWinOrDraw() {
	const char (*bd)[9] = &ticTacToeGame.board;
	std::vector<std::vector<int>> wins = {
		{0,1,2}, {3,4,5}, {6,7,8}, // rows
		{0,3,6}, {1,4,7}, {2,5,8}, // cols
		{0,4,8}, {2,4,6}           // diags
	};

	// Check for a winner
	for (auto &w : wins) {
		if ((*bd)[w[0]] == (*bd)[w[1]] && 
			(*bd)[w[1]] == (*bd)[w[2]]) 
		{
			std::string winner(1, (*bd)[w[0]]);
			return "We have a splendid victor: " + winner + 
				" (applause!) 🤯🖕🍆🤏 \nBoard:\n" + renderBoard();
		}
	}

	// Check for draw (i.e. all squares taken but no winner)
	bool draw = true;
	for (int i = 0; i < 9; ++i) {
		if ((*bd)[i] != 'X' && (*bd)[i] != 'O') {
			draw = false;
			break;
		}
	}
	if (draw)
		return "It's a draw! (╯°□°）╯︵ ┻━┻🖕🍆🤏 \n" + renderBoard();

	// No winner, no draw -> return empty string
	return "";
}

// Modify your onMessage callback
void onMessage(std::string user, std::string channel, std::string message)
{
	// Only pay attention to #gpt channel
	if (channel != "#tictactoe") return;

	Bot &bot = getBot();

	// Basic commands
	if (message == "!tictactoe") {
		if (ticTacToeGame.active) {
			bot.sendMessage(channel, "A game is already in progress, dear " + user + 
										". 🏅 Try finishing that before starting anew.");
			return;
		}
		// Start a new game
		ticTacToeGame = TicTacToeGame(); 
		ticTacToeGame.active = true;
		bot.sendMessage(channel, "Tic-Tac-Toe has commenced, Mr. Elsher 🕵️‍♂️! " 
								"Type `!join` to jump in. 🖕🍆🤏\n" + 
								std::string(TICTACTOE_HELP_JOKE));
		return;
	}

	if (message == "!join") {
		if (!ticTacToeGame.active) {
			bot.sendMessage(channel, "No game in progress, " + user + 
										". Pray start one with `!tictactoe`. (҂◡_◡) 🖕🍆🤏");
			return;
		}

		// If X is free, user becomes X
		if (ticTacToeGame.xPlayer.empty()) {
			ticTacToeGame.xPlayer = user;
			bot.sendMessage(channel, "Splendid, " + user + 
										", thou art 'X'. (ง •_•)ง 🖕🍆🤏");
		}
		// else if O is free, user becomes O
		else if (ticTacToeGame.oPlayer.empty() && user != ticTacToeGame.xPlayer) {
			ticTacToeGame.oPlayer = user;
			bot.sendMessage(channel, "Magnificent, " + user + 
										", thou art 'O'. (｡◕‿◕｡) 🖕🍆🤏");
		}
		else {
			bot.sendMessage(channel, "Alas, both roles (X and O) have been assigned. " 
										"You must watch from the grandstands, " + user + ".");
		}
		return;
	}

	// e.g. "!move 5"
	if (message.rfind("!move ", 0) == 0) {
		if (!ticTacToeGame.active) {
			bot.sendMessage(channel, "No game in progress, " + user + 
										". Please start one with `!tictactoe`");
			return;
		}
		if (ticTacToeGame.xPlayer.empty() || ticTacToeGame.oPlayer.empty()) {
			bot.sendMessage(channel, "We haven't two players yet, dear " + user + 
										". Summon an opponent forthwith!");
			return;
		}

		// Check if it's the correct player's turn
		bool isXTurn = (ticTacToeGame.currentTurn == 'X');
		std::string currentPlayer = (isXTurn) ? ticTacToeGame.xPlayer 
											: ticTacToeGame.oPlayer;
		if (user != currentPlayer) {
			bot.sendMessage(channel, "Not your turn, " + user + 
										"! Patience, good sir or madam. 🍵🖕🍆🤏");
			return;
		}

		// Parse desired position
		int position = -1;
		try {
			position = std::stoi(message.substr(6)) - 1; // "!move " is 6 chars
		} catch (...) {
			bot.sendMessage(channel, "Invalid move command. Use `!move 1` to `!move 9`, e.g. `!move 5` (≧◡≦) 🖕🍆🤏");
			return;
		}

		if (position < 0 || position > 8) {
			bot.sendMessage(channel, "Position out of bounds, " + user + 
										"! Choose from 1 to 9. (∩｀-´)⊃━☆ 🖕🍆🤏");
			return;
		}

		// Check if space is taken
		if (ticTacToeGame.board[position] == 'X' || ticTacToeGame.board[position] == 'O') {
			bot.sendMessage(channel, "That space is already occupied, " + user + 
										"! Please pick a free location. 🚧🖕🍆🤏");
			return;
		}

		// Perform move
		ticTacToeGame.board[position] = (isXTurn) ? 'X' : 'O';

		// Show board
		std::string boardStr = renderBoard();
		bot.sendMessage(channel, "Board updated, " + user + 
									" made a move! 📡\n" + boardStr);

		// Check for winner
		std::string result = checkWinOrDraw();
		if (!result.empty()) {
			bot.sendMessage(channel, result);
			// End game
			ticTacToeGame.active = false;
			return;
		}

		// Switch turns
		ticTacToeGame.currentTurn = (isXTurn) ? 'O' : 'X';
		return;
	}

	// Otherwise, if no recognized command, fallback to original GPT logic:
	bot.sendMessage(channel, bot.ApiCall(message));
}

// custom function to handle disconnect
void onDisconnect()
{
	std::cerr << "Disconnected from server!" << std::endl;
}

void onUserChannelJoin(std::string user, std::string channel)
{
	std::cout << "User " << user << " joined channel " << channel << std::endl;

	getBot().directMessage(user, "Welcome " + user + " to the channel! Just type anything and pgt will answer.");
}

void onUserChannelLeave(std::string user, std::string channel)
{
	std::cout << "User " << user << " left channel " << channel << std::endl;

	getBot().directMessage(user, "Goodbye " + user + "! Have a nice day!");
}
