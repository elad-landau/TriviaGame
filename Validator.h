#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <iterator>


using namespace std;

class validator 
{
public:
	static bool isPasswordValid(string password);
	static bool isUsernameValid(string username);

};

