#include "Application.hpp"

#ifndef BOARD_DEBUG
#include "Windows.h"
#endif


int main(int, char**)
{
	#ifndef BOARD_DEBUG
	FreeConsole();
	#endif
	
	board::Application app;
	return app.Start();
}