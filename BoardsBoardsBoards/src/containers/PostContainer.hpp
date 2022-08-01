#pragma once

#include <unordered_map>
#include <string>

#include "LuaVector.hpp"
#include "renderables/posts/Post.hpp"
#include "utils/BoardColors.hpp"

using std::string;
using std::unordered_map;
using utils::BoardColors;

namespace board
{
	class PostContainer
	{
	public:
		
		using iterator = LuaVector<Post>::iterator;				
		iterator begin() { return posts.begin(); }
		iterator end() { return posts.end(); }
		iterator IteratorFromIndex(std::size_t pos);
		iterator Insert(iterator pos, const Post& post);
		iterator Insert(iterator pos, Post&& post);
		iterator CreatePost(iterator pos, const std::string& content); 
		iterator CreatePost(iterator pos, std::string&& content = ""); 	
		iterator CreatePostBack(const std::string& content);
		iterator CreatePostBack(std::string&& content = "");
		iterator Erase(iterator pos);
		iterator MoveToLastPosition(iterator pos);
		iterator MoveToLastPosition(std::size_t pos);


		using const_iterator = LuaVector<Post>::const_iterator;
		const_iterator begin() const { return posts.begin(); }
		const_iterator end() const { return posts.end(); }
		const_iterator IteratorFromIndex(std::size_t pos) const;
		const Post& operator[](int count) const { return posts.at(count); };
		std::size_t size() const { return posts.size(); }

		
		bool operator==(const PostContainer& rhs) const;
		Post& operator[](int count) { return posts.at(count); };
		void Clear() { posts.Clear(); }
		void PopBack();
		void Resize(int count);
		bool Empty() const { return posts.Empty(); }


		struct BoardOptions
		{
			BoardColors color_table;

			bool operator==(const BoardOptions& rhs) const
			{
				const bool color_equals = color_table == rhs.color_table;
				return color_equals;
			}

		}board_options;

		struct PostConnection
		{
			PostConnection(std::size_t from = 0, std::size_t to = 0) : from(from), to(to) {}
			std::size_t from;
			std::size_t to;

			bool operator==(const PostConnection& rhs) const
			{
				return (from == rhs.from) && (to == rhs.to);
			}
		};

		LuaVector<PostConnection> connections = LuaVector<PostConnection>(false);

	private:

		LuaVector<Post> posts = LuaVector<Post>(false);

		void PropagateIndexShift(iterator start, int offset, std::size_t removed_node_idx = 0);
		void PropagateIndexShift(iterator start, iterator end, int offset, std::size_t removed_node_idx = 0);
		void ReconnectNodes(std::unordered_map<std::size_t, std::size_t>& index_changes);
	};
}