#pragma once

#include "containers/PostContainer.hpp"
#include "ScriptParser.hpp"
#include "ParsingStrategies.hpp"

namespace utils
{
	using sb::PostContainer;
	using sb::Post;

	class BoardParser : public ScriptParser<PostContainer>
	{
	public:
		BoardParser(LuaTableIntoContainer parseTable = ParsingStrategies::TableToContainer_Default, ContainerIntoLuaTable parseContainer = ParsingStrategies::ContainerToTable_Default) : ScriptParser<PostContainer>(parseTable, parseContainer) {}
	};
}