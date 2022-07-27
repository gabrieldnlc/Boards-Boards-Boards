#pragma once

#include <string>

#include "ParsingHelpers.hpp"
#include "containers/LuaVector.hpp"
#include "containers/PostContainer.hpp"
#include "renderables/posts/PostContent.hpp"
#include "utils/BoardColors.hpp"
#include "utils/Error.hpp"

namespace utils
{
	using board::PostContainer;
	using board::Post;
	using board::LuaVector;
	using board::PostContent;
	using board::ContentType;
	using std::string;

	using PostContentIntoLuaTable = std::function<sol::table(const LuaVector<PostContent>&)>;
	using LuaObjectIntoPost = std::function<Post(sol::object)>;
	using LuaTableIntoContainer = std::function<PostContainer(sol::table)>;
	using ContainerIntoLuaTable = std::function<sol::table(const PostContainer&)>;

	class ParsingStrategies
	{
	public:

		static const PostContentIntoLuaTable PostContentToTable;

		static const LuaObjectIntoPost ObjectToPost;

		static const LuaTableIntoContainer TableToContainer;

		static const ContainerIntoLuaTable ContainerToTable;
	};
		
	


}