#include <iostream>
#include <fstream>
#include <sstream>

#include "utils/Error.hpp"
#include "LuaStack.hpp"

//TODO the order of the functions

namespace utils
{
	sol::table LuaStack::EmptyTable() //TODO check if this creates the table on the stack and if it persists
	{
		return sol::table(lua_state, sol::create);
	}
	string LuaStack::StringFromFile(const string& path)
	{
		std::ifstream input_file(path);
		if (!input_file.is_open())
		{
			throw FileOpenError(path);
		} //TODO more in-depth error throwing
		return std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
	}

	void LuaStack::GlobalToFile(const std::string& global_table, const string& path)
	{
		sol::table global = GetGlobalTable(global_table);
		TableToFile(global, path);
	}

	void LuaStack::TableToFile(const sol::table& table, const string& path)
	{
		std::string serialized = TableToString(table);
		std::ofstream file(path, std::fstream::out);
		if (!file) throw FileWriteError("Could not open " + path + " for writing.");
		file << serialized;
	}

	string LuaStack::GlobalTableToString(const string& global_table)
	{
		sol::table table = GetGlobalTable(global_table);
		return TableToString(table);	
	}

	std::string LuaStack::TableToString(const sol::table& table)
	{
		std::string serialized = PrettyStringFromTable(table);
		return serialized;
	}

	std::string LuaStack::PrettyStringFromTable(const sol::table& table)
	{
		sol::table serpent = GetGlobalTable("serpent");
		if (!serpent.valid()) {
			throw LuaAccessError("Serpent is not loaded");
		}
		if (!table.valid()) {
			throw LuaAccessError("Not a valid table");
		}
		sol::function pretty = serpent["block"];

		sol::table options = LuaStack::EmptyTable();
		options["comment"] = false;

		std::string serialized = pretty(table, options); //TODO error catching

		return serialized;
	}

	sol::table LuaStack::DeserializeTableString(const string& serialized_table)
	{
		sol::table serpent = GetGlobalTable("serpent");
		if (!serpent.valid()) {
			throw LuaAccessError("Serpent is not loaded");
		}
		sol::function deserialize = serpent["load"];

		sol::table options = LuaStack::EmptyTable();
		options["safe"] = true;
		try
		{
			std::pair<bool, sol::table> t = deserialize(serialized_table, options);
			if (!t.first) throw DeserializingError(); // Weird but necessary
			return t.second;
		}
		catch (const std::exception&)
		{
			throw ParsingError("Not a valid script file.");
		}
		
	}

	sol::table LuaStack::TableFromFile(const string& path)
	{
		string serialized = "return " + StringFromFile(path);
		return DeserializeTableString(serialized);
	}

	void LuaStack::Init()
	{
		LoadBaseLibs();
		lua_state.require_file("serpent", "scripts/serpent/src/serpent.lua");
	}

	void LuaStack::FlushStack()
	{
		lua_state = sol::state();
		Init();
	}

	void LuaStack::ScriptToGlobal(const string& path)
	{
		lua_state.script_file(path); //TODO error catching
	}

	sol::object LuaStack::Require(const string& key, const string& path)
	{
		return lua_state.require_file(key, path);
	}
	
	sol::object LuaStack::GetVariableFromStack(const string& object_name)
	{
		auto object = lua_state[object_name];
		if (!object.valid())
		{
			throw LuaAccessError("Object '" + object_name + "' does not exist in the current stack");
		}
		return object.get<sol::object>();
	}
	
	void LuaStack::LoadBaseLibs()
	{
		lua_state.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::table, sol::lib::string);
	}

	
	sol::table LuaStack::GetGlobalTable(const string& table_name)
	{
		auto table = lua_state[table_name];
		if (!table.valid())
		{
			throw LuaAccessError("Table '" + table_name + "' does not exist in the current stack");
		}
		return table.get<sol::table>();
	}
	
}