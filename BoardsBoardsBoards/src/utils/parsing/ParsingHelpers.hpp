#pragma once

#include <functional>
#include <exception>
#include <vector>
#include <string>

#include "utils/Error.hpp"
#include "utils/LuaStack.hpp"

namespace utils
{
	const auto VariantFromLuaObject = [](sol::object obj) -> std::variant<int, float, bool, std::string>
	{
		std::variant<int, float, bool, std::string> var;
		switch (obj.get_type())
		{
		case (sol::type::boolean):
			var = (obj.as<bool>());
			break;
		case (sol::type::string):
			var = (obj.as<std::string>());
			break;
		case (sol::type::number):
			if (obj.is<int>())
			{
				var = obj.as<int>();
			}
			else
			{
				var = obj.as<float>();
			}
			break;
		default:
			throw ParsingError("Unknown tag type.");
		}
		return var;
	};

	const auto VariantsFromLuaObject = [](sol::object obj) ->std::vector<std::variant<int, float, bool, std::string>>
	{
		std::vector<std::variant<int, float, bool, std::string>> vars;
		switch (obj.get_type())
		{
		case (sol::type::table):
		{
			sol::table t = obj.as<sol::table>();
			for (std::size_t i = 1; i <= t.size(); i++)
			{
				vars.emplace_back(VariantFromLuaObject(t[i]));
			}
		}
		break;
		default:
			vars.emplace_back(VariantFromLuaObject(obj));
			break;
		}
		return vars;
	};
}