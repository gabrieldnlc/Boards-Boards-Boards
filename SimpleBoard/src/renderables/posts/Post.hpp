#pragma once

#include <memory> // unique_ptr
#include <string>

#include "containers/LuaVector.hpp"
#include "Tags.hpp"
#include "PostContent.hpp"

namespace sb
{

	class Post
	{
		
	public:

		Post(std::string text = "") : content(true)
		{
			content.EmplaceBack(std::move(text));
		}
		Post(const std::vector<std::string>& str_vector) : content(true)
		{
			for (auto& str : str_vector)
			{
				content.EmplaceBack(str);
			}
		}

		bool operator== (const Post& another_post) const
		{
			bool same_content = content == another_post.content;
			bool same_tags = tags == another_post.tags;
			bool same_display_pos = display_pos == another_post.display_pos;
			bool same_color = (color[0] == another_post.color[0]) && (color[1] == another_post.color[1]) && (color[2] == another_post.color[2]);

			return (same_content && same_tags && same_display_pos && same_color);

			/*
			return (content == another_post.content) && (tags == another_post.tags) &&
				(display_pos == another_post.display_pos) && (color == another_post.color);
				*/
		}

		std::size_t GetIdx() const { return Idx; }

		void SetIdx(std::size_t new_idx) { Idx = new_idx; }

		void IndexShift(int offset) { Idx += offset; }
		
		LuaVector<PostContent> content;
		std::size_t editing_content = 0;
		std::pair<float, float> display_pos;
		float color[3]{ -1.f, -1.f, -1.f };
		Tags tags;		
		
	private:
		std::size_t Idx = 0; // if 0, it is not inserted in PostContainer
		
	};
}