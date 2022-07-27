#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

namespace board
{
	class FilePath
	{
	public:
		FilePath(std::string path_, char delimiter = '\\') 
			: path(std::move(path_)), delimiter(delimiter)
		{ //TODO better handling of fringe cases
			if (path.empty())
			{
				start_of_level = path.data();
				return;
			}
			if (path.find(delimiter, 0) == std::string::npos)
			{

			}
			if (path.size() == 1)
			{
				if (path[0] == delimiter)
				{
					pos.push_back(0);
				}
				setPointerToLevel();
				return;
			}
			if (path.back() == delimiter)
			{
				this->path = path.substr(0, path.size() - 2);
			}
			calculateSplitPositions();
			setPointerToLevel();
		}
		void setLevel(std::size_t level) 
		{
			if (level <= 0)
			{
				current_level = 0;
				setPointerToLevel();
				return;
			}
			if (level >= pos.size() - 1)
			{
				current_level = pos.size() - 1;
				start_of_level = path.data();
				return;
			}
			current_level = level;
			setPointerToLevel();
		}
		bool empty() const
		{
			return path.empty();
		}
		void goUp()
		{
			if (path.empty()) return;
			if (current_level >= pos.size() - 1) return;
			setLevel(current_level + 1);
		}
		void goDown()
		{
			if (path.empty()) return;
			if (current_level == 0) return;
			setLevel(current_level - 1);
		}
		const char* data() const
		{
			//return start_of_level;
			return path.data() + pos[current_level];
		}
		const char* fullData() const
		{
			return path.data();
		}
		bool isSliced() const
		{
			return pos.empty();
		}
		bool isLastLevel() const
		{
			return current_level == pos.size() - 1;
		}
		bool isFirstLevel() const
		{
			return current_level == 0;
		}
		const char* GetFirstLevel() const
		{
			return path.data() + pos[0] + 1;
		}

	private:
		void setPointerToLevel()
		{
			if (pos.empty() || pos.size() == 1)
			{
				start_of_level = path.data();
				return;
			}
			start_of_level = path.data() + pos[current_level] + 1;
		}
		void calculateSplitPositions()
		{
			pos.push_back(0);
			std::size_t i = path.find(delimiter, 0);
			if (i != std::string::npos)
			{
				if (i != 0)
				{
					pos.insert(std::begin(pos), i);
				}
				i = path.find(delimiter, i + 1);
				while (i != std::string::npos)
				{
					pos.insert(std::begin(pos), i);
					i = path.find(delimiter, i + 1);
				}
			}
		}
		char* start_of_level;
		std::string path;
		std::vector<std::size_t> pos;
		std::size_t current_level = 0;
		char delimiter;
	};
}