#pragma once


class Game;
class Room;

#include <string>
#include <iostream>

#include "Game.h"

#include "Room.h"
#include "Helper.h"
#include "Protocol.h"

#include <vector>

#define WIN32_LEAN_AND_MEAN

using namespace std;

class User
{
public:
	User(string username, SOCKET sock);
	void send(string message);

	string getUsername();
	SOCKET getSock();
	Room* getCurrRoom();
	Game* getCurrGame();

	void setCurrGame(Game* game);
	void setCurrRoom(Room* room);

	void clearRoom();
	bool createRoom(int id, string name, int maxUsers, int questionsNo, int questionTime);
	bool joinRoom(Room* room);
	void leaveRoom();
	int closeRoom();
	bool leaveGame();

private:
	string _username;
	SOCKET _sock;
	Game *_currGame;
	Room *_currRoom;
};