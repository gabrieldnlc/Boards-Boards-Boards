#pragma once

#include <memory> // unique_ptr
#include <string>

#include "containers/LuaVector.hpp"
#include "Tags.hpp"
#include "PostContent.hpp"

namespace board
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
			const bool same_content = content == another_post.content;
			const bool same_tags = tags == another_post.tags;
			const bool same_display_pos = display_pos == another_post.display_pos;
			const bool same_color = (color[0] == another_post.color[0]) && (color[1] == another_post.color[1]) && (color[2] == another_post.color[2]);

			return (same_content && same_tags && same_display_pos && same_color);

		}

		std::size_t GetIdx() const { return Idx; }

		void SetIdx(std::size_t new_idx) { Idx = new_idx; }

		void IndexShift(int offset) { Idx += offset; }

		bool HasColor()
		{
			return (color[0] >= 0 && color[1] >= 0 && color[2] >= 0);
		}
		
		LuaVector<PostContent> content;
		std::size_t editing_content = 0;
		std::pair<float, float> display_pos;
		float color[3]{ -1.f, -1.f, -1.f };
		Tags tags;		
		
	private:
		std::size_t Idx = 0; // if 0, it is not inserted in PostContainer
		
	};
}