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

		Post(std::string text = "")
		{
			content.EmplaceBack(std::move(text));
		}

		Post(const std::vector<std::string>& str_vector)
		{
			for (auto& str : str_vector)
			{
				content.EmplaceBack(str);
			}
		}

		bool operator== (const Post& rhs) const
		{
			const bool same_content = content == rhs.content;
			const bool same_tags = tags == rhs.tags;
			const bool same_display_pos = display_pos == rhs.display_pos;
			const bool same_color = (color[0] == rhs.color[0]) && (color[1] == rhs.color[1]) && (color[2] == rhs.color[2]);

			return (same_content && same_tags && same_display_pos && same_color);

		}

		bool HasColor() const
		{
			return (color[0] >= 0 && color[1] >= 0 && color[2] >= 0);
		}


		std::size_t GetIdx() const { return Idx; }
		void SetIdx(std::size_t new_idx) { Idx = new_idx; }
		void IndexShift(int offset) { Idx += offset; }

		
		LuaVector<PostContent> content = LuaVector<PostContent>(true);
		std::size_t editing_content = 0;
		std::pair<float, float> display_pos;
		float color[3]{ -1.f, -1.f, -1.f };
		Tags tags;		
		
	private:
		std::size_t Idx = 0; // If 0, Post is not inserted into PostContainer
		
	};
}