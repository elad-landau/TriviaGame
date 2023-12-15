#pragma once

#include <string>
#include <iostream>
#include <WinSock2.h>
#include <Windows.h>


#include "Helper.h"
#include "Protocol.h"

#include <vector>
#include <map>
#include <iterator>
#include <queue>

using namespace std;

class Question
{
public:
	Question(int id, std::string question, std::string correctAnswer, std::string answer2, std::string answer3, std::string answer4);

	string getQuestion();
	string* getAnswers();
	int getCorrectAnswerIndex();
	int getId();
private:
	string _question;
	string _answers[4];
	int _correctAnswerIndex;
	int _id;
};

