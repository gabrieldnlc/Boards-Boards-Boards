#include "Application.hpp"

#ifdef BOARD_RELEASE
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