#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "User.h"
#include "Question.h"
#include "DataBase.h"

class User;

class Game
{
public:
	Game(const vector<User*>& players, int questionsNo,	 DataBase *db);
	~Game();

	void handleFinishGame();
	void sendFirstQuestion();
	bool handleNextTurn();
	bool handleAnswerFromUser(User* user, int answerNo, int time);
	bool leaveGame(User* currUser);

	int getID();


private:
	DataBase* db;
	vector<Question*> _questions;
	vector<User*> _players;
	int _question_no;
	int _currQuestionIndex;
	map<string, int> _results;
	int _currentTurnAnswers;
	int _id;

	bool insertGameToDB();
	void initQuestionsFromDB();
	void sendQuestionToAllUsers();
};
