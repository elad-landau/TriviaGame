#include "Game.h"

Game::Game(const vector<User*>& players, int questionsNo, DataBase* db) : db(db), _question_no(questionsNo)
{
	this->_id = db->insertNewGame();

	this->_players = players;

	//this->_question_no = 0;
	this->_currQuestionIndex = 0;
	this->_currentTurnAnswers = 0;
	this->_questions = db->initQuestions(questionsNo);


	for (vector<User*>::iterator it = this->_players.begin(); it != this->_players.end(); it++)
	{
		(*it)->setCurrGame(this);
		this->_results.insert(pair<string,int>((*it)->getUsername(), 0));
	}
}

Game::~Game()
{
	/*
	for (std::vector<User*>::iterator it = this->_players.begin(); it != this->_players.end(); ++it)
	{
		delete (*it);
	}
	*/ //we need the users also after a game ends
	//אבל עדיין צריך לנקות את גיים, השחקנים שמורים גם במקום אחר
	this->_players.clear();

	for (std::vector<Question*>::iterator it = this->_questions.begin(); it != this->_questions.end(); ++it)
	{
		delete (*it);
	}
	this->_questions.clear();
}

void Game::handleFinishGame()
{
	string msg = "", currPlayer;
	db->updateGameStatus(this->_id);

	msg += Helper::getPaddedNumber(END_GAME, 3) + Helper::getPaddedNumber(this->_players.size(), 1);

	for (int i = 0; i < this->_players.size(); ++i)
	{
		currPlayer = this->_players[i]->getUsername();
		msg += Helper::getPaddedNumber(currPlayer.size(), 2) + currPlayer;
		msg += Helper::getPaddedNumber(this->_results.at(currPlayer), 2);
	}

	for (std::vector<User*>::iterator it = this->_players.begin(); it != this->_players.end(); ++it)
	{
		(*it)->setCurrGame(nullptr);
		try
		{
			(*it)->send(msg);
		}
		catch (const std::exception& ex)
		{
			cout << ex.what() << endl;
		}
	}
}

void Game::sendFirstQuestion()
{
	this->sendQuestionToAllUsers();
}

bool Game::handleNextTurn()
{
	if (this->_players.empty()) {
		this->handleFinishGame();
		return false;
	}
	if (this->_players.size() == this->_currentTurnAnswers)
	{
		if (_currQuestionIndex == this->_question_no)
		{
			this->handleFinishGame();
			return false;
		}
		else
		{
			this->sendQuestionToAllUsers();
			return true;
		}
	}
	return true;
}

bool Game::handleAnswerFromUser(User * user, int answerNo, int time)
{
	bool isCurrect = false;
	_currentTurnAnswers++;

	if (answerNo == this->_questions[this->_currQuestionIndex-1]->getCorrectAnswerIndex() +1 )
	{
		isCurrect = true;
		this->_results.at(user->getUsername())++;
		user->send(to_string(IS_RIGHT_ANSWER * 10 + 1));
	}

	else
		user->send(to_string(IS_RIGHT_ANSWER * 10));

	if (answerNo == 5)
		this->db->addAnswerToPlayer(this->_id, user->getUsername(), this->_currQuestionIndex, "", isCurrect, time);
	else
		this->db->addAnswerToPlayer(this->_id, user->getUsername(), this->_currQuestionIndex, this->_questions[this->_currQuestionIndex-1]->getAnswers()[answerNo-1], isCurrect, time);
	//if(_currentTurnAnswers == _players.size() || _players.empty())
		return this->handleNextTurn();
}

bool Game::leaveGame(User * currUser)
{
	for (std::vector<User*>::iterator it = this->_players.begin(); it != this->_players.end(); ++it)
	{
		if (currUser->getUsername() == (*it)->getUsername())
		{
			this->_players.erase(it);
			return this->handleNextTurn();

		}
	}
	return false;
}

int Game::getID()
{
	return this->_id;
}


bool Game::insertGameToDB()
{
	//Function unneessary
	return false;
}

void Game::initQuestionsFromDB()
{
	//Function unneessary
	return;
}

void Game::sendQuestionToAllUsers()
{
	string msg = to_string(SEND_QUESTIONS);


	msg += Helper::getPaddedNumber(this->_questions[this->_currQuestionIndex]->getQuestion().size(),3) + this->_questions[this->_currQuestionIndex]->getQuestion();


	for (int i = 0; i < 4; i++)
		msg += Helper::getPaddedNumber((this->_questions[this->_currQuestionIndex]->getAnswers()[i]).size(),3) + (this->_questions[this->_currQuestionIndex]->getAnswers()[i]);
	
	this->_currQuestionIndex++;
	this->_currentTurnAnswers = 0;

	for (std::vector<User*>::iterator it = this->_players.begin(); it != this->_players.end(); it++)
	{
		try
		{
			(*it)->send(msg);
		}
		catch (const std::exception& ex)
		{
			cout << ex.what() << endl;
			(*it)->getCurrRoom()->getAdmin()->send(to_string(SEND_QUESTIONS*10));
		}
	}
}
