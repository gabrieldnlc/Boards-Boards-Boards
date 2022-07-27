#pragma once

#include <functional>
#include <string>

#include "sol/sol.hpp"

#include "utils/LuaStack.hpp"

namespace utils
{
	template<typename T>
	class ScriptParser
	{

	public:
		using TableParsingStrategy = std::function<T(sol::table)>;
		using ToTableStrategy = std::function<sol::table(const T&)>;

		ScriptParser(TableParsingStrategy parsing, ToTableStrategy to_table) : ParseTable(parsing), ReturnToTable(to_table) {}
		T ParsePath(const std::string& path)
		{
			sol::table table = LuaStack::TableFromFile(path);
			return Parse(table);
		}
		T Parse(sol::table table)
		{
			return ParseTable(table);
		}

		sol::table ToTable(const T& parse)
		{
			return ReturnToTable(parse);
		}
		void SetParsingStrategy(TableParsingStrategy strategy)
		{
			ParseTable = strategy;
		}
		void SetToTableStrategy(ToTableStrategy strategy)
		{
			ReturnToTable = strategy;
		}
	private:
		TableParsingStrategy ParseTable;
		ToTableStrategy ReturnToTable;
	};
}