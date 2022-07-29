#pragma once

#include <string>
#include <vector>

namespace utils
{
	using std::string;
	using std::vector;
	using std::size_t;

	class FilePath
	{
		// - This class serves two functions: 
		//  it holds the complete path to the file that is being edited and
		//  AtCurrentLevel() will show only the relevant data for displaying the tab name.

#ifdef WIN32
		static const char d = '\\';
#else
		static const char d = '/';
#endif

	public:
		FilePath(const char* path, char delimiter = d) : FilePath(string(path), delimiter) {}
		FilePath(const string& path, char delimiter = d) : fullPath(path), delimiter(delimiter)
		{
			if (path.size() <= 1 || path.find(delimiter, 0) == std::string::npos)
			{
				return;
			}

			if (fullPath.back() == delimiter)
			{
				fullPath = fullPath.substr(0, path.size() - 2);
			}

			size_t i = 0;
			size_t pos = fullPath.find(delimiter, i);

			while (pos != string::npos)
			{
				levels.push_back(pos);
				i = pos + 1;
				pos = fullPath.find(delimiter, i);
			}

			curr_level = levels.size() - 1;
		}

		const string& GetFullPath() const { return fullPath; }
		const char* AtCurrentLevel() const { return fullPath.data() + levels[curr_level] + 1; }

		size_t GetCurrentLevel() const { return curr_level; }
		size_t GetTotalLevels() const { return levels.size(); }
		bool LevelUp()
		{
			if (curr_level + 1 >= levels.size()) return false;
			curr_level++;
			return true;
		}
		bool LevelDown()
		{
			if (curr_level == 0) return false;
			curr_level--;
			return true;
		}

	private:
		string fullPath;
		size_t curr_level = 0;
		vector<int> levels{ -1 };
		char delimiter;
	};
}