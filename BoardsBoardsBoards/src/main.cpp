#include "Application.hpp"

#ifdef BOARD_RELEASE
#include "Windows.h"
#endif


int main(int, char**)
{
	#ifdef BOARD_RELEASE
	FreeConsole();
	#endif
	
	board::Application app;
	return app.Start();
}