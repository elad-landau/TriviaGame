#include "User.h"

User::User(string username, SOCKET sock)
{
	this->_username = username;
	this->_sock = sock;
	this->_currGame = nullptr;
	this->_currRoom = nullptr;
}

string User::getUsername()
{
	return this->_username;
}

SOCKET User::getSock()
{
	return this->_sock;
}

void User::setCurrRoom(Room* room)
{
	this->_currRoom = room;
}

Room * User::getCurrRoom()
{
	return this->_currRoom;
}

Game * User::getCurrGame()
{
	return this->_currGame;
}

void User::setCurrGame(Game * game)
{
	this->_currGame = game;
}

void User::send(string message)
{
	Helper::sendData(this->_sock, message);
}

void User::clearRoom()
{
	this->_currRoom = nullptr;
}

bool User::createRoom(int id, string name, int maxUsers, int questionsNo, int questionTime)
{
	if (this->_currRoom != nullptr) {
		this->send("1141");
		return false;
	}
	this->_currRoom = new Room(id, this, name, maxUsers, questionsNo, questionTime);
	this->send("1140");
	return true;
}

bool User::joinRoom(Room * room)
{
	if (this->_currRoom != nullptr) {
		return false;
	}
	return room->joinRoom(this);;
}

void User::leaveRoom()
{
	if (this->_currRoom != nullptr)
	{
		this->_currRoom->leaveRoom(this);
	}
	this->_currRoom = nullptr;
}

int User::closeRoom()
{
	int id;
	if (this->_currRoom == nullptr)
		return -1;

	id = this->_currRoom->closeRoom(this);
	delete this->_currRoom;
	this->_currRoom = nullptr;
	return id;
}

bool User::leaveGame()
{
	bool rtn = false;
	if(this->_currGame == nullptr)
		return rtn;

	rtn = this->_currGame->leaveGame(this);
	this->_currGame = nullptr;
	return rtn;
}
