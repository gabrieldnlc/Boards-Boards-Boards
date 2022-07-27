#pragma once

#include <string>
#include <vector>

#include "utils/Error.hpp"

namespace board
{
	class FileDialog
	{
	public:
		static std::string Open();
		static std::vector<std::string> OpenMultiple();
		static std::string Save();
	};
}