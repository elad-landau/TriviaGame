#include "RecievedMessage.h"

RecievedMessage::RecievedMessage(int msgCode, SOCKET user_socket, vector<string>* moreDeatils)
{
	this->_msgCode = msgCode;
	this->_user_socket = user_socket;
	this->_moreDetails = moreDeatils;
}

RecievedMessage::~RecievedMessage()
{
	this->_moreDetails->clear();
	delete[] this->_moreDetails;
}



void RecievedMessage::setUser(User* user)
{
	this->_user = user;
}

User * RecievedMessage::getUser()
{
	return this->_user;
}


SOCKET RecievedMessage::getSocket()
{
	return _user_socket;
}

int RecievedMessage::getMsgCode()
{
	return _msgCode;
}

vector<string> RecievedMessage::getMoreDetails()
{
	return *this->_moreDetails;
}
