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
		BoardParser(LuaTableIntoContainer parseTable = ParsingStrategies::TableToContainer, ContainerIntoLuaTable parseContainer = ParsingStrategies::ContainerToTable) : ScriptParser<PostContainer>(parseTable, parseContainer) {}
	};
}