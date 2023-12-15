#include "TriviaServer.h"

TriviaServer::TriviaServer()
{

	_roomIdSequence = 0;

	db = new DataBase();

	lock = new unique_lock<mutex>(Msg_queue, defer_lock);

	WSADATA wsa_data = {};
	if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
		throw std::exception("WSAStartup Failed");


	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_socket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__ " - socket");

	
}

TriviaServer::~TriviaServer()
{
	try
	{
		delete lock;
		db->~DataBase();
		WSACleanup();
	}
	catch (...) {}

	//TRACE(__FUNCTION__ " closing accepting socket");
	try
	{
		// the only use of the destructor should be for freeing 
		// resources that was allocated in the constructor
		closesocket(_socket);
	}
	catch (...) {}
}

void TriviaServer::serve()
{
	thread tr(&TriviaServer::handleRecievedMessages, this);
	tr.detach();
	try
	{
		bindAndListen();
	}

	catch (...)
	{
		cout << "exception" << endl;
	}
	while (true)
		accept_msg();
}

void TriviaServer::bindAndListen()
{

	struct sockaddr_in sa = { 0 };
	sa.sin_port = htons(PORT);
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = IFACE;
	// again stepping out to the global namespace
	if (::bind(_socket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - bind");
//	TRACE("binded");

	if (::listen(_socket, SOMAXCONN) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - listen");
//	TRACE("listening...")


}

void TriviaServer::accept_msg()
{
	SOCKET client_socket = accept(_socket, NULL, NULL);
	if (client_socket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__);

	//TRACE("Client accepted !");
	// create new thread for client	and detach from it
	thread* tr = new thread(&TriviaServer::clientHandler, this, client_socket);
	tr->detach();

}

void TriviaServer::clientHandler(SOCKET client_socket)
{
	int msgType = Helper::getMessageTypeCode(client_socket);
	while (msgType != 0  && msgType != EXIT_APP)
	{
		try
		{
			buildRecieveMessage(client_socket, msgType);
			msgType = Helper::getMessageTypeCode(client_socket);
		}
		catch (const exception& ex)
		{
			cout << "exception at client Handler : "<< ex.what() << endl;
		}
	}
	/*
	made end message and push it to the queue
	*/
}

void TriviaServer::buildRecieveMessage(SOCKET client_socket, int msgCode)
{

	vector<string>* moreDetails = new vector<string>;
	switch (msgCode)
	{
	case SIGN_IN:
		moreDetails->push_back(Helper::getStringPartFromSocket(client_socket, atoi(Helper::getPartFromSocket(client_socket, 2, 0))));//username
		moreDetails->push_back(Helper::getStringPartFromSocket(client_socket, atoi(Helper::getPartFromSocket(client_socket, 2, 0))));//password
		break;

	case SIGN_UP:

		moreDetails->push_back(Helper::getStringPartFromSocket(client_socket, atoi(Helper::getPartFromSocket(client_socket, 2, 0))));//username
		moreDetails->push_back(Helper::getStringPartFromSocket(client_socket, atoi(Helper::getPartFromSocket(client_socket, 2, 0))));//password
		moreDetails->push_back(Helper::getStringPartFromSocket(client_socket, atoi(Helper::getPartFromSocket(client_socket, 2, 0))));//email

		//cout << (*moreDetails)[0] << (*moreDetails)[1] << (*moreDetails)[2];
		//system("PAUSE");

		break;

	case GET_ROOMS_USERS:
	case JOIN_REQUEST:
		moreDetails->push_back(Helper::getStringPartFromSocket(client_socket, 4));//roomID
		break;
	case CREATE_ROOM:
		moreDetails->push_back(Helper::getStringPartFromSocket(client_socket, atoi(Helper::getPartFromSocket(client_socket, 2, 0))));//room name 
		moreDetails->push_back(Helper::getStringPartFromSocket(client_socket, 1));//playersNumber  
		moreDetails->push_back(Helper::getStringPartFromSocket(client_socket, 2));//questionsNumber  
		moreDetails->push_back(Helper::getStringPartFromSocket(client_socket, 2));//questionTimeInSec  

		break;
	case SEND_ANSWER:
		moreDetails->push_back(Helper::getStringPartFromSocket(client_socket, 1));//answerNumber   
		moreDetails->push_back(Helper::getStringPartFromSocket(client_socket, 2));//TimeInSeconds  
		break;
	default:
		delete moreDetails;
		moreDetails = NULL;
		break;
	}

	RecievedMessage* newMsg = new RecievedMessage(msgCode, client_socket, moreDetails);
	try
	{
		addRecievedMessage(newMsg);
	}
	catch (...)
	{
		cout << "exception" << endl;
	}
}

void TriviaServer::addRecievedMessage(RecievedMessage *msg)
{

	lock->lock();
	_queRcvMessages.push(msg);
	lock->unlock();

	try
	{
		//unique_lock<mutex> cond_locker(cond_m);
		queue_var.notify_all();
		//cond_locker.~unique_lock();
	}

	catch (...)
	{
		cout << "exception at addRecievedMessage" << endl;
	}
}

void TriviaServer::handleRecievedMessages()
{
	srand(time(NULL));
	
	RecievedMessage* msg;
	User* newUser;

	while (true)
	{
		unique_lock<mutex> cond_locker(cond_m);
		if(_queRcvMessages.size() == 0)
			queue_var.wait(cond_locker);
		//cond_locker.~unique_lock();

		lock->lock();
		msg = _queRcvMessages.front();
		_queRcvMessages.pop();
		lock->unlock();

		//msg->setUser(getUserBySocket(msg->getSocket()));


		switch (msg->getMsgCode())
		{
		case SIGN_IN:
			newUser = handleSignin(msg);
			if (newUser != nullptr)
			{
				this->_connectedUsers.insert(make_pair(msg->getSocket(), newUser));
			}
			break;

		case SIGN_OUT:
			msg->setUser(TriviaServer::getUserBySocket(msg->getSocket()));
			handleSingout(msg);
			break;

		case SIGN_UP:
			if (!handleSignup(msg))
			{
				cout << "problem with SIGN UP" << endl;
				//messages were send in the handle
			}
			break;

			
		case CREATE_ROOM:
			msg->setUser(TriviaServer::getUserBySocket(msg->getSocket()));
			if (!handleCreateRoom(msg))
			{
				if (msg->getUser())
					msg->getUser()->send(to_string(CREATE_ROOM_RES * 10 + 1));
			}
			else
				msg->getUser()->send(to_string(CREATE_ROOM_RES * 10));
			break;
			

		case JOIN_REQUEST:
			msg->setUser(TriviaServer::getUserBySocket(msg->getSocket()));
			if (handleJoinRoom(msg))
			{
				Room* room = this->getRoomById(atoi(msg->getMoreDetails()[0].c_str())); // send user's list to all users in room
				vector<User*> users = room->getUsers();

				for (vector<User*>::iterator it = users.begin();it != users.end();it++)
				{
					vector<string> *details = new vector<string>;
					details->push_back(msg->getMoreDetails()[0]);
					RecievedMessage *currMsg = new RecievedMessage(GET_ROOMS_USERS, (*it)->getSock(), details);
					addRecievedMessage(currMsg);
				}
			}
			break;

		case GET_EXISTING_ROOMS:
			msg->setUser(TriviaServer::getUserBySocket(msg->getSocket()));
			handleGetRooms(msg);
			break;

		case LEAVE_ROOM:
			msg->setUser(TriviaServer::getUserBySocket(msg->getSocket()));
			if (handleLeaveRoom(msg))
			{
				cout << "problem with leaveRoom" << endl;
			}
			break;

		case CLOSE_ROOM:
			msg->setUser(TriviaServer::getUserBySocket(msg->getSocket()));
			if (!handleCloseRoom(msg))
			{
				cout << "problem with close room" << endl;
			}
			break;
			
		case GET_ROOMS_USERS:
			msg->setUser(TriviaServer::getUserBySocket(msg->getSocket()));
			handleGetUsersInRoom(msg);
			break;
			
		case START_GAME:
			msg->setUser(TriviaServer::getUserBySocket(msg->getSocket()));
			handleStartGame(msg);
			break;

		case SEND_ANSWER:
			msg->setUser(TriviaServer::getUserBySocket(msg->getSocket()));
			handlePlayerAnswer(msg);
			break;

		case PERSONAL_DATA:
			msg->setUser(TriviaServer::getUserBySocket(msg->getSocket()));
			handleGetPersonalStatus(msg);
			break;

		case LEAVE_GAME:
		default:
			msg->setUser(TriviaServer::getUserBySocket(msg->getSocket()));
			safeDeleteUser(msg);
			break;
		}
		//delete msg;
	}
}

Room * TriviaServer::getRoomById(int roomId)
{
	if (this->_roomList.find(roomId) == this->_roomList.end())
		return nullptr;
	return this->_roomList[roomId];
}

User * TriviaServer::getUserByName(string username)
{
	for (map<SOCKET, User*>::iterator it = this->_connectedUsers.begin(); it != this->_connectedUsers.end(); it++)
	{
		if (it->second->getUsername() == username)
			return it->second;
	}
	return nullptr;
}

User* TriviaServer::getUserBySocket(SOCKET client_socket)
{
	if (this->_connectedUsers.find(client_socket) == this->_connectedUsers.end())
		return nullptr;
	return this->_connectedUsers[client_socket];
}

void TriviaServer::handleSingout(RecievedMessage * msg)
{
	User *user = msg->getUser();
	if (user != NULL) {
		this->_connectedUsers.erase(user->getSock());

		user->closeRoom();
		user->leaveRoom();
		user->leaveGame();
		user->send(Helper::getPaddedNumber(LEAVE_ROOM_RES, 4));
	}
}

bool TriviaServer::handleSignup(RecievedMessage * msg)
{
	/*****/
	User *newUser = new User((msg->getMoreDetails()[0]), msg->getSocket());
	msg->setUser(newUser);
	/*****/

	if (!validator::isUsernameValid(msg->getMoreDetails()[0])) {
		msg->getUser()->send(to_string(USERNAME_ILLEGAL));
		delete newUser;
		return false;
	}
	if (!validator::isPasswordValid(msg->getMoreDetails()[1])) {
		msg->getUser()->send(to_string(PASSWORD_ILLEGAL));
		delete newUser;
		return false;
	}
	if (db->isUserExists(msg->getMoreDetails()[0])) {
		msg->getUser()->send(to_string(USERNAME_EXISTS));
		delete newUser;
		return false;
	}
	if (!db->addNewUser(msg->getMoreDetails()[0], msg->getMoreDetails()[1], msg->getMoreDetails()[2])) {
		msg->getUser()->send(to_string(SINGUP_ERRUR));
		delete newUser; 
		return false;
	}
	
	msg->getUser()->send(to_string(SIGN_UP_RES * 10));
	//_connectedUsers.insert(pair<SOCKET, User*>(msg->getSocket(), newUser));
	return true;
}

void TriviaServer::safeDeleteUser(RecievedMessage* msg)
{
	handleSingout(msg);

	try
	{
		closesocket(msg->getSocket());
	}
	catch (const std::exception& ex)
	{
		cout << ex.what() << endl;
	}
}

User * TriviaServer::handleSignin(RecievedMessage * msg)
{
	
	User *user = new User(msg->getMoreDetails()[0], msg->getSocket());

	if (!(db->isUserAndPassMatch(msg->getMoreDetails()[0], msg->getMoreDetails()[1]))) {
		user->send(to_string(WRONG_DETAILS));
		delete user;
		return nullptr;
	}


	if (this->getUserBySocket(msg->getSocket()) != nullptr) {
		user->send(to_string(ALREADY_CONNECTED));
		delete user;
		return nullptr;
	}
	
	msg->setUser(user);
	
	user->send(to_string(SIGN_IN_RES*10));
	return user;
}

bool TriviaServer::handleCreateRoom(RecievedMessage * msg)
{
	User* curr = msg->getUser();
	if (curr == nullptr)
		return false;

	vector<string> details = msg->getMoreDetails();
	this->_roomIdSequence++;

	if (curr->createRoom(this->_roomIdSequence, details[0], atoi(details[1].c_str()), atoi(details[2].c_str()), atoi(details[3].c_str())))
	{
		_roomList.insert(pair<int, Room*>(this->_roomIdSequence, curr->getCurrRoom()));
		return true;
	}

	this->_roomIdSequence--;
	return false;
}

bool TriviaServer::handleCloseRoom(RecievedMessage * msg)
{
	Room* roomToClose = msg->getUser()->getCurrRoom();
	int room_id = -1;
	if (roomToClose == nullptr)
		return false;
	room_id = roomToClose->closeRoom(msg->getUser());
	if (room_id == -1)
		return false;

	_roomList.erase(room_id);
	_roomIdSequence--;
	msg->getUser()->clearRoom();
	return true;
}

bool TriviaServer::handleJoinRoom(RecievedMessage * msg)
{
	User* user = msg->getUser();
	if (user == nullptr)
		return false;
	Room* room = this->getRoomById(atoi(msg->getMoreDetails()[0].c_str()));
	if (room == nullptr) {
		user->send(to_string(ROOM_NOT_EXIST));
		return false;
	}
	return room->joinRoom(user);
}

bool TriviaServer::handleLeaveRoom(RecievedMessage * msg)
{
	User* user = msg->getUser();
	if (user == nullptr)
		return false;
	Room* room = user->getCurrRoom();
	if (room == nullptr) {
		return false;
	}
	room->leaveRoom(user);
	//alrady in LeaveRoom
	//user->send(to_string(LEAVE_ROOM_RES));
	return true;
}

void TriviaServer::handleStartGame(RecievedMessage *msg)
{
	Room *currRoom = msg->getUser()->getCurrRoom();
	Game* game = new Game(currRoom->getUsers(), currRoom->getQuestionNo(), db);
	vector<User*> users = currRoom->getUsers();
	try
	{
		for (vector<User*>::iterator it = users.begin(); it != users.end(); it++)
				(*it)->clearRoom();

		_roomList.erase(currRoom->getId());
		_roomIdSequence--;
		
	}
	catch (const std::exception& ex)
	{
		cout << ex.what() << endl;
	}

	game->sendFirstQuestion();
}

void TriviaServer::handleGetUsersInRoom(RecievedMessage * msg)
{
	User* curr = msg->getUser();
	Room* room = TriviaServer::getRoomById(atoi(msg->getMoreDetails()[0].c_str()));
	if (room == nullptr)
	{
		curr->send(to_string(SEND_ROOMS_USERS * 10));
		return;
	}

	curr->send(room->getUsersListMessage());
}

void TriviaServer::handleGetRooms(RecievedMessage * msg)
{
	ostringstream oss;
	oss << SEND_EXISTING_ROOMS << Helper::getPaddedNumber(_roomList.size(), 4);
	for (map<int, Room*>::iterator it = _roomList.begin();it != _roomList.end();it++)
	{
		oss << Helper::getPaddedNumber(it->first, 4);
		oss << Helper::getPaddedNumber(it->second->getName().length(), 2);
		oss << it->second->getName();
	}

	msg->getUser()->send(oss.str());
}

void TriviaServer::HandleLeaveGame(RecievedMessage *msg)
{
	Game* game = msg->getUser()->getCurrGame();
	if (!msg->getUser()->leaveGame())
	{
		game->~Game();
	}
}

void TriviaServer::handlePlayerAnswer(RecievedMessage *msg)
{
	User* user = msg->getUser();
	Game* game = user->getCurrGame();
	if (user->getCurrGame() != nullptr)
	{
		if (!game->handleAnswerFromUser(user, atoi(msg->getMoreDetails()[0].c_str()), atoi(msg->getMoreDetails()[1].c_str())) )
			game->~Game();
	}
}

void TriviaServer::handleGetPersonalStatus(RecievedMessage *msg)
{
	if (msg->getUser() == nullptr)
		throw exception("cannot get personal status without user log in");
	db->getPersonalStatus(msg->getUser()->getUsername());
}