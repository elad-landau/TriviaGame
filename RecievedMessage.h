#pragma once

#include "Helper.h"
#include "User.h"

#include <string>
#include <iostream>
#include <WinSock2.h>
#include <Windows.h>

#include <vector>
#include <iterator>
#include <map>
#include <queue>
#include <iomanip>
#include <sstream>



class RecievedMessage
{
private:
	int _msgCode;
	SOCKET _user_socket;
	vector<string>* _moreDetails;
	User* _user;

public:
	RecievedMessage(int msgCode, SOCKET user_socket, vector<string>* moreDeatils);
	~RecievedMessage();
	
	void setUser(User* user);
	User* getUser();
	SOCKET getSocket();
	int getMsgCode();
	vector<string> getMoreDetails();
};

