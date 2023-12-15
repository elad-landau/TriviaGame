#pragma once

#include <string>
#include <iostream>
#include <WinSock2.h>
#include <Windows.h>

#include "User.h"
#include "Helper.h"
#include "Protocol.h"

#include <vector>

class User;

using namespace std;

class Room
{
public:
	
	Room(int id, User* admin, string name, int maxUsers, int questionsNo, int questionTime);

	string getUsersListMessage();
 	bool joinRoom(User* user);
	void leaveRoom(User* user);
	int closeRoom(User* user);

	int getQuestionNo();
	string getName();
	User *getAdmin();
	vector<User*> getUsers();
	int getId();



private:
	string getUsersAsString(vector<User*> usersList, User* excludeUser);
	void sendMessage(User* excludeUser, string message);
	void sendMessage(string message);

	vector<User*> _users;
	User* _admin;
	int _maxUsers;
	int _questionTime;
	int _questionNo;
	string _name;
	int _id;
};

