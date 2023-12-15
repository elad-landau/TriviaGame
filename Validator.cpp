#include "Validator.h"

bool validator::isPasswordValid(string password)
{
	if (password.length() < 4)
		return false;

	size_t count_lower = count_if(password.begin(), password.end(),
		[](unsigned char ch) { return islower(ch); });
	if (count_lower <= 0)
		return false;

	size_t count_upper = count_if(password.begin(), password.end(),
		[](unsigned char ch) { return isupper(ch); });
	if (count_upper <= 0)
		return false;

	size_t count_digit = count_if(password.begin(), password.end(),
		[](unsigned char ch) { return isdigit(ch); });
	if (count_digit <= 0)
		return false;

	size_t count_blank = count_if(password.begin(), password.end(),
		[](unsigned char ch) { return isblank(ch); });
	if (count_blank > 0)
		return false;

	return true;
}

bool validator::isUsernameValid(string username)
{
	if (username.length() <= 0)
		return false;

	size_t count_blank = count_if(username.begin(), username.end(),
		[](unsigned char ch) { return isblank(ch); });
	if (count_blank > 0)
		return false;

	return isalpha(username[0]) != 0;
}