#pragma once

#include <unordered_map>
#include <string>

#include "LuaVector.hpp"
#include "renderables/posts/Post.hpp"
#include "utils/ColorTable.hpp"

namespace sb
{
	using std::string;
	using std::unordered_map;
	using utils::ColorTable;

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

		Post& operator[](int count) { return posts.at(count); };
		void Clear() { posts.Clear(); }
		
		void PopBack();
		void Resize(int count);
		bool Empty() const { return posts.Empty(); }

		using const_iterator = LuaVector<Post>::const_iterator;

		const_iterator begin() const { return posts.begin(); }
		const_iterator end() const { return posts.end(); }
		const Post& operator[](int count) const { return posts.at(count); };
		std::size_t size() const { return posts.size(); }
		bool operator==(const PostContainer& c2) const;
		const_iterator IteratorFromIndex(std::size_t pos) const;

		struct BoardOptions
		{
			ColorTable color_table;

			bool operator==(const BoardOptions& another_board) const
			{
				return color_table == another_board.color_table;
			}

		}board_options;

	private:
		LuaVector<Post> posts = LuaVector<Post>(false);
		void PropagateIndexShift(iterator start, int offset, std::size_t removed_node_idx = 0);
		void PropagateIndexShift(iterator start, iterator end, int offset, std::size_t removed_node_idx = 0);
		void ReconnectNodes(std::unordered_map<std::size_t, std::size_t>& index_changes);
	};
}