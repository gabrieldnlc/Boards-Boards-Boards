#pragma once

#include <string>
#include <vector>

#include "utils/Error.hpp"

namespace board
{
	using std::string;
	using std::vector;

	class FileDialog
	{
	public:
		
		static string Open();
		static vector<string> OpenMultiple();
		static string Save();
	};
}