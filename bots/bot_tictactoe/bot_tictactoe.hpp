#ifndef BOT_TICTACTOE_HPP
#define BOT_TICTACTOE_HPP

#include "../../bot_template/Bot.hpp"
#include <iostream>
#include <curl/curl.h>
#include <string>
#include <vector>
#include <regex>
#include <vector>
#include <sstream>

class Bot_TicTacToe : public Bot
{
	private:
		std::string			_username_player1;
		std::string			_username_player2;
		bool				_send_back_as_privmsg;
		bool				_send_back_in_channel;
		bool				_game_is_won = true;
		std::vector<char>	_board;
		char				_currentPlayer;
		bool				_gameOver;
		int					_movesCount;
	
	public:
		void		TicTacToe(int move);
		void		ticTacToeReset();
		bool		checkWin();
		std::string getBoardStr() const;
};

#endif