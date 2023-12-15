#pragma once

#include "sqlite3.h"
#include "Question.h"

#include <exception>
#include <string>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <time.h>

#define DB_NAME "trivia.db"
#define USER_TABLE_FULL "t_users(username,password,email)"
#define USER_TABLE "t_users"
#define QUES_TABLE "t_questions"
#define GAME_TABLE "t_games"
#define GAME_TABLE_FULL "t_games(status,start_time,end_time)"
#define PLAY_ANS_TABLE "t_players_answers"
#define PLAY_ANS_TABLE_FULL "t_players_answers(game_id,username,question_id,player_answer,is_correct,answer_time)" 

using namespace std;


class DataBase
{
public:
	DataBase();
	~DataBase();
	bool isUserExists(string username);
	bool addNewUser(string username, string password, string email);
	bool isUserAndPassMatch(string username, string password);
	vector<Question*> initQuestions(int questionsNo);
	int insertNewGame();
	bool updateGameStatus(int roomNum);
	bool addAnswerToPlayer(int gameId, string username, int questionId, string answer, bool isCorrect, int answerTime);
	vector<string> getPersonalStatus(string username);

private:
	sqlite3* db;
	
	static int getValue_call_back(void* notUsed, int argc, char** argv, char** azCol);
	static vector<string> getValue(sqlite3* db, string tableName, string PK, string PKName, string colName, char* zErrMsg);
	static int callSql(char * sql, sqlite3* db);
	static int getCount(string PKName, string table, sqlite3* db, char *zErrMsg);
	static int show_callback(void* notUsed, int argc, char** argv, char** azCol);
	int getNumOfGames(string username);

};
