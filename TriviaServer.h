#pragma once
#include <string>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <WinSock2.h>
#include <Windows.h>


#include "RecievedMessage.h"
#include "User.h"
#include "DataBase.h"
#include "Room.h"
#include "Helper.h"
#include "Protocol.h"
#include "Validator.h"
#include "DataBase.h"

#include <vector>
#include <map>
#include <iterator>
#include <queue>


using namespace std;


#define PORT 8820
static const unsigned int IFACE = 0;

class TriviaServer
{
private:
	map<SOCKET, User*> _connectedUsers;
	map<int, Room*> _roomList;
	queue<RecievedMessage*> _queRcvMessages;
	int _roomIdSequence;
	SOCKET _socket;

	mutex Msg_queue,cond_m;
	unique_lock<mutex> *lock;
	condition_variable queue_var;

	DataBase *db;

	//listen all the time for new connections
	void bindAndListen();

	//accept new clients
	void accept_msg();

	//the thread, take care of all what necessary
	void clientHandler(SOCKET client_socket);

	//safe add the msg to the queue
	void addRecievedMessage(RecievedMessage* msg);

	//construce a recievedMessage and send it to addRecievedMessage
	void buildRecieveMessage(SOCKET client_socket, int msgCode);




public:
	TriviaServer();
	~TriviaServer();

	//call for bind and listen. make new thread forCLientHandler
	void serve();
	
	Room* getRoomById(int roomId);
	User* getUserByName(string username);
	User* getUserBySocket(SOCKET client_socket);

	//the thread that handle the queue message
	void handleRecievedMessages();

	//sign out the user and close the socket
	void safeDeleteUser(RecievedMessage* msg);
	User* handleSignin(RecievedMessage *msg);
	void handleSingout(RecievedMessage *msg);
	bool handleSignup(RecievedMessage *msg);
	void HandleLeaveGame(RecievedMessage *msg);
	void handleStartGame(RecievedMessage *msg);
	void handlePlayerAnswer(RecievedMessage *msg);
	bool handleCreateRoom(RecievedMessage *msg);
	bool handleCloseRoom(RecievedMessage *msg);
	bool handleJoinRoom(RecievedMessage *msg);
	bool handleLeaveRoom(RecievedMessage *msg);
	void handleGetUsersInRoom(RecievedMessage *msg);
	void handleGetRooms(RecievedMessage *msg);
	void handleGetBestScores(RecievedMessage *msg);
	void handleGetPersonalStatus(RecievedMessage *msg);

	

};




