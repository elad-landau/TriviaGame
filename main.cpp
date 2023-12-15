#include <iostream>
#include "TriviaServer.h"

#pragma comment(lib, "Ws2_32.lib") 

int main(void)
{
	TriviaServer* triv = new TriviaServer();

	triv->serve();
	
//	delete triv;
	return 0;
}