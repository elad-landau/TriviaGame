#include "Question.h"



Question::Question(int id, string question, string correctAnswer, string answer2, string answer3, string answer4)
{
	this->_id = id;
	this->_question = question;
	int arr[4];
	bool flag;


	int randInt;


	for (int i = 0; i < 4; i++)
	{
		flag = true;
		while (flag)
		{
			flag = false;
			randInt = rand() % 4;
			for (int j = 0;j < i;j++)
			{
				if (randInt == arr[j])
					flag = true;
			}
		}

		arr[i] = randInt;
		switch (randInt)
		{
			
		case (0) :		
			this->_answers[i] = correctAnswer;
			this->_correctAnswerIndex = i;
			break;
		case (1) :
			this->_answers[i] = answer2;
			break;
		case (2) :
			this->_answers[i] = answer3;
			break;		
		case (3) :
			this->_answers[i] = answer4;
			break;
		default:
			break;
		}

	}
	
}

string Question::getQuestion()
{
	return this->_question;
}


string* Question::getAnswers()
{
	return this->_answers;
}


int Question::getCorrectAnswerIndex()
{
	return this->_correctAnswerIndex;
}


int Question::getId()
{
	return this->_id;
}
