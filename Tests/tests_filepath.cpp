#include <string>

#include "catch.hpp"

#include "utils/FilePath.hpp"

using namespace utils;
using std::string;

const string tag = "[FilePath]";

SCENARIO("A FilePath initialized from string without delimiters will always show full string", tag)
{
	GIVEN("FilePath initialized with string without delimiters")
	{
		FilePath path = "New Tab (1)";
		THEN("Its current level will always be 0, LevelDown() and LevelUp() return false")
		{

			REQUIRE(path.GetFullPath() == "New Tab (1)");
			REQUIRE(path.GetFullPath() == path.AtCurrentLevel());
			REQUIRE(path.GetCurrentLevel() == 0);

			REQUIRE_FALSE(path.LevelDown());
			REQUIRE(string(path.AtCurrentLevel()) == "New Tab (1)");
			REQUIRE_FALSE(path.LevelUp());
			REQUIRE(string(path.AtCurrentLevel()) == "New Tab (1)");
		}
	}
}

SCENARIO("A FilePath initialized from string with delimiters will behave to specifications", tag)
{
	GIVEN("A string representing the location of a file")
	{
		string path = "C:\\Users\\Testing\\file.ext";

		WHEN("A FilePath instance is initialized with that string")
		{
			FilePath p = path;

			REQUIRE(p.GetTotalLevels() == 4);
			REQUIRE(p.GetFullPath() == path);

			THEN("LevelDown() and LevelUp() will always stay in the allowed range, returning false when at edge")
			{
				REQUIRE(string(p.AtCurrentLevel()) == "file.ext");
				REQUIRE(p.LevelUp());
				REQUIRE(string(p.AtCurrentLevel()) == "Testing\\file.ext");
				REQUIRE(p.LevelUp());
				REQUIRE(string(p.AtCurrentLevel()) == "Users\\Testing\\file.ext");
				REQUIRE(p.LevelUp());
				REQUIRE(string(p.AtCurrentLevel()) == "C:\\Users\\Testing\\file.ext");

				REQUIRE_FALSE(p.LevelUp());
				REQUIRE(p.AtCurrentLevel() == p.GetFullPath());

				REQUIRE(p.LevelDown());
				REQUIRE(string(p.AtCurrentLevel()) == "Users\\Testing\\file.ext");
				REQUIRE(p.LevelDown());
				REQUIRE(string(p.AtCurrentLevel()) == "Testing\\file.ext");
				REQUIRE(p.LevelDown());
				REQUIRE(string(p.AtCurrentLevel()) == "file.ext");
				REQUIRE_FALSE(p.LevelDown());
			}
			AND_THEN("GoToLastLevel() and GoToFirstLevel() respect the string boundaries")
			{
				p.GoToLastLevel();
				REQUIRE(string(p.AtCurrentLevel()) == "C:\\Users\\Testing\\file.ext");

				p.GoToFirstLevel();
				REQUIRE(string(p.AtCurrentLevel()) == "file.ext");
			}
		}
	}
}