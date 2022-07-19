#pragma once

#include <nfd.h>
//#include <stdio.h> TODO: necessary?
//#include <stdlib.h>
#include <string>
#include <vector>

#include "utils/Error.hpp"

namespace sb
{
	class FileDialog
	{
	public:
		static std::string Open();
		static std::vector<std::string> OpenMultiple();
		static std::string Save();
	};
}