#pragma once

#include <string>

#include "sol/sol.hpp"

namespace utils
{
	using std::string;

	class LuaStack
	{
		
	public:

		//--- BASIC STACK OPERATIONS ---
		static void        Init();
		static void        FlushStack();
		static void        ScriptToGlobal(const string& path);
		static sol::object Require(const string& key, const string& path);
		//--- BASIC STACK OPERATIONS ---
 
		//--- LUA STACK ACCESS ---
		static sol::object GetVariableFromStack(const string& object_name);
		static sol::table  GetGlobalTable(const string& table_name); 
		static sol::table  EmptyTable();
		//--- LUA STACK ACCESS ---
		
		//--- FILE I/O ---
		static void        TableToFile(const sol::table& table, const string& path);
		static void        GlobalToFile(const string& global_table, const string& path);
		static string	   StringFromFile(const string& path);
		static sol::table  TableFromFile(const string& path);
		//--- FILE I/O --- 

		//--- SERIALIZATION
		static string GlobalTableToString(const string& global_table);
		static string TableToString(const sol::table& table);
		static string PrettyStringFromTable(const sol::table& table);
		static sol::table  DeserializeTableString(const string& serialized_table);
		//--- SERIALIZATION
	private:
		static inline sol::state lua_state;
		static void LoadBaseLibs();
	};
}