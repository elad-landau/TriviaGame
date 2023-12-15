#include "Room.h"

Room::Room(int id, User* admin, string name, int maxUsers, int questionsNo, int questionTime)
{
	this->_id = id;
	this->_admin = admin;
	this->_users.push_back(admin);
	this->_name = name;
	this->_maxUsers = maxUsers;
	this->_questionNo = questionsNo;
	this->_questionTime = questionTime;
}

string Room::getUsersListMessage()
{
	string mess108 = to_string(SEND_ROOMS_USERS);
	mess108 += to_string(this->_users.size());
	if (mess108 == to_string(SEND_ROOMS_USERS*10))
		return mess108;
	for (vector<User*>::iterator it = this->_users.begin(); it != this->_users.end(); it++)
	{
		mess108 += Helper::getPaddedNumber((*it)->getUsername().length(), 2);
		mess108 +=  (*it)->getUsername();
	}
	return mess108;
}

bool Room::joinRoom(User * user)
{
	string str = to_string(JOIN_REQUEST_RES);
	if (this->_users.size() == this->_maxUsers)
	{
		user->send(str +'1');
		return false;
	}
	else
	{
		try
		{
			this->_users.push_back(user);
		}
		catch (const std::exception&)
		{
			user->send(str + '2');
			return false;
		}
	}
	str += '0';
	str += Helper::getPaddedNumber(this->_questionNo,2)  + Helper::getPaddedNumber( this->_questionTime,2);
	user->send(str);
	user->setCurrRoom(this);
	return true;
}

void Room::leaveRoom(User * user)
{
	User* leaveUser = user;
	for (vector<User*>::iterator it = this->_users.begin(); it != this->_users.end(); it++)
	{
		if ((*it)->getUsername() == user->getUsername())
		{
			this->_users.erase(it);
			leaveUser->send(to_string(LEAVE_ROOM_RES));
			this->sendMessage(leaveUser, this->getUsersListMessage());
			return;
		}
	}
}

int Room::closeRoom(User * user)
{
	if(user->getUsername() != this->_admin->getUsername())
		return -1;
	for (vector<User*>::iterator it = this->_users.begin(); it != this->_users.end(); it++)
	{
		(*it)->send(to_string(CLOSE_ROOM_RES));
		if ((*it)->getUsername() != _admin->getUsername())
		{
			(*it)->clearRoom();
		}
	}
	return this->_id;
}

int Room::getQuestionNo()
{
	return this->_questionNo;
}

string Room::getName()
{
	return this->_name;
}

User* Room::getAdmin()
{
	return this->_admin;
}

vector<User*> Room::getUsers()
{
	return this->_users;
}

int Room::getId()
{
	return this->_id;
}

string Room::getUsersAsString(vector<User*> usersList, User * excludeUser)
{
	string str = "";
	int i = 1;
	for (vector<User*>::iterator it = usersList.begin(); it != usersList.end(); it++, i++)
	{
		if ((*it)->getUsername() != excludeUser->getUsername())
		{
			str += "player No. " + i;
			str += "is: " + (*it)->getUsername();
			str += "\n";
		}
		i--;
	}
	return str;
}

void Room::sendMessage(User * excludeUser, string message)
{
	for (vector<User*>::iterator it = this->_users.begin(); it != this->_users.end(); it++)
	{
		if ((*it)->getUsername() != excludeUser->getUsername())
		{
			try
			{
				(*it)->send(message);
			}
			catch (const exception&)
			{
				//Helper::sendData();
				/* למלא את זה לפי סנד של יוזר*/
			}
		}
	}
}

void Room::sendMessage(string message)
{
	this->sendMessage(NULL, message);
}
