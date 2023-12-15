#include "DataBase.h"

DataBase::DataBase() 
{
	int rc = sqlite3_open(DB_NAME, &db);
	if (rc)
	{
		throw std::exception("cant open data base");
		sqlite3_close(db);
	}
	
}

DataBase::~DataBase()
{
	sqlite3_close(db);
}


int DataBase::getValue_call_back(void* notUsed, int argc, char** argv, char** azCol)
{
	if (((vector<string>*)notUsed)->size() == 0)
		((vector<string>*)notUsed)->push_back(argv[0]);
	else
	{
		if (((vector<string>*)notUsed)->back() != argv[0])
			((vector<string>*)notUsed)->push_back(argv[0]);
	}
	
	return 0;
}


vector<string> DataBase::getValue(sqlite3* db, string tableName, string PK, string PKName, string colName, char* zErrMsg)
{
	stringstream s;
	vector<string> str;

	s << "select " << colName << " from " << tableName << " where " << PKName << " = " << PK << ";" << endl;

	sqlite3_exec(db, s.str().c_str(), DataBase::getValue_call_back, &str, &zErrMsg);
	return str;
}

int DataBase::getCount(string PKName, string table, sqlite3* db, char *zErrMsg)
{
	ostringstream oss;
	string str;
	oss << "select " << PKName << ", COUNT(*) from " << table << "; ";

	sqlite3_exec(db, oss.str().c_str(), DataBase::getValue_call_back, &str, &zErrMsg);
	return(atoi(str.c_str()));
}


int DataBase::show_callback(void* notUsed, int argc, char** argv, char** azCol)
{

	int i;

	for (i = 0; i < argc; i++)
		cout << azCol[i] << " = " << argv[i] << endl;

	cout << "****callback****" << endl;
	return 0;
}


int DataBase::callSql(char * sql, sqlite3* db)
{
	int rc;
	char * zErrMsg;

	rc = sqlite3_exec(db, sql,show_callback, 0, &zErrMsg);

	if (rc)
	{
		cout << "problem :  " << zErrMsg << endl;		
		return 1;
	}
	return 0;
}


bool DataBase::isUserExists(string username)
{
	char * zErrMsg = 0;
	ostringstream oss;
	oss << "\""<<username<<"\"";

	string s = oss.str();
	vector<string> info = DataBase::getValue(db, USER_TABLE, s, "username", "password", zErrMsg);

	if (info.front().compare("") == 0)
		return false;

	return true;
}

bool DataBase::addNewUser(string username, string password, string email)
{
	ostringstream ss;
	ss << "insert into " << USER_TABLE_FULL << " values (\"" << username << "\",\"" << password << "\",\"" << email << "\");";

	string s = ss.str();
	char * sqlLine = new char[s.length() + 1];
	strcpy(sqlLine, s.c_str());
	 
	if ( DataBase::callSql(sqlLine,db) == 1)
	{
		delete []sqlLine;
		return false;
	}
	delete []sqlLine;
	return true;
}


bool DataBase::isUserAndPassMatch(string username, string password)
{
	char * zErrMsg = 0;

	ostringstream oss;
	oss << "\"" << username << "\"";

	string s = oss.str();

	if (!isUserExists(username))
	{
		throw std::exception("Error while searching in DB. user dont exist");
		return false;
	}

	string pass = getValue(db, USER_TABLE, s, "username", "password", zErrMsg).front();
	
	return (pass.compare(password) == 0);
}


vector<Question*> DataBase::initQuestions(int questionsNo)
{
	vector<Question*> vec;
	//vector<string> info;
	char* zErrMsg = 0;
	bool flag;
	
	int QuesNum = DataBase::getCount("question_id", QUES_TABLE, db, zErrMsg);
	if (QuesNum < questionsNo)
		questionsNo = QuesNum;

	for (int i = 0;i < questionsNo;i++)
	{
		int currNum;
		flag = true;
		Question *ques;
		string question,right, ans2, ans3, ans4;

		while (flag)
		{
			flag = false;
			currNum = rand() % QuesNum +1;

			for (int j = 0;j < i;j++)
			{
				if (currNum == vec.at(j)->getId())
					flag = true;
			}
		}

		question = getValue(db, QUES_TABLE, to_string(currNum), "question_id", "question", zErrMsg).front();
		right = getValue(db, QUES_TABLE, to_string(currNum), "question_id", "correct_ans", zErrMsg).front();
		ans2 = getValue(db, QUES_TABLE, to_string(currNum), "question_id", "ans2", zErrMsg).front();
		ans3 = getValue(db, QUES_TABLE, to_string(currNum), "question_id", "ans3", zErrMsg).front();
		ans4 = getValue(db, QUES_TABLE, to_string(currNum), "question_id", "ans4", zErrMsg).front();

		ques = new Question(currNum, question, right, ans2, ans3, ans4);
		vec.push_back(ques);
	}

	return vec;
}


int DataBase::insertNewGame()
{
	//status 0
	// time NOW

	string str;
	char * zErrMsg = 0;
	ostringstream oss;
	oss << "insert into " << GAME_TABLE_FULL << " values(0,CURRENT_TIMESTAMP ,NULL);";
	string dum = oss.str();
	char * sqlLine = new char[dum.length() + 1];
	strcpy(sqlLine,dum.c_str());

	callSql(sqlLine, db);
	sqlite3_exec(db, "select last_insert_rowid();", DataBase::getValue_call_back, &str, &zErrMsg);

	delete[]sqlLine;
	return atoi(str.c_str());
}


bool DataBase::updateGameStatus(int roomNum)
{
	ostringstream oss;
	oss << "update " << GAME_TABLE << " set status = 1 where game_id = " << roomNum << ";";
	string str = oss.str();

	char * sqlLine = new char[str.length() + 1];
	strcpy(sqlLine, str.c_str());
	if (callSql(sqlLine, db))
	{
		delete[]sqlLine;
		return false;
	}
	delete[]sqlLine;

	oss << "update " << GAME_TABLE << " set end_time = CURRENT_TIMESTAMP where game_id = " << roomNum << ";";
	str = oss.str();

	sqlLine = new char[str.length() + 1];
	strcpy(sqlLine, str.c_str());
	if (callSql(sqlLine, db))
	{
		delete[]sqlLine;
		return false;
	}
	delete[]sqlLine;
	return true;
}

bool DataBase::addAnswerToPlayer(int gameId, string username, int questionId, string answer, bool isCorrect, int answerTime)
{
	int correct = isCorrect ? 1 : 0;
	ostringstream oss;
	oss << "insert into " << PLAY_ANS_TABLE_FULL << " values(" << gameId << ",\"" << username << "\"," << questionId << ",\"" << answer << "\"," << correct << "," << answerTime << ");";
	string str = oss.str();
	char* sqlLine = new char[str.length() + 1];
	strcpy(sqlLine, str.c_str());

	return !callSql(sqlLine, db);

}

vector<string> DataBase::getPersonalStatus(string username)
{
	vector<string> info;	
	//num of right answers
	//num of wrong answers
	//average answer time

	info.push_back(to_string(getNumOfGames(username))); //num of games

	return info;
}

int DataBase::getNumOfGames(string username)
{
	char * zErrMsg = 0;

	ostringstream oss;
	oss << '\"' << username << '\"';

	vector<string> info = DataBase::getValue(db, PLAY_ANS_TABLE, oss.str().c_str(), "username", "game_id", zErrMsg);
	
	return info.size();
}