#include "BoardTab.hpp"

#include "imgui.h"
#include "imgui_stdlib.h"

#include "renderables/DearImGuiFlags.hpp"
#include "renderables/posts/PostContent.hpp"
#include "utils/ColorTable.hpp"

using utils::ColorTable;

namespace sb
{
	BoardTab::BoardTab(string unique_ID) : Widget(unique_ID), path(std::move(unique_ID)), posts_info(true) {}
	BoardTab::BoardTab(PostContainer&& container, string path) : container(std::move(container)), Widget(path), path(std::move(path)), posts_info(true) {}
	BoardTab::BoardTab(const PostContainer& container, string path) : container(container), Widget(path), path(std::move(path)), posts_info(true) {}

	void RenderText(Post& post, std::size_t content_idx, std::pair<ImRect, ImRect>& content_rects)
	{
		PostContent& content = post.content[content_idx];
		auto& text = content.AsString();
		bool is_editing = content_idx == post.editing_content;

		auto& displaying_rect = content_rects.first;
		auto& editing_rect = content_rects.second;
		bool editing_rect_empty = editing_rect.GetWidth() <= 0;

		if (is_editing)
		{
			if (editing_rect_empty)
			{
				editing_rect = displaying_rect;
				editing_rect.Expand(ImVec2(10, 16));
			}
			void* ptr = &text;
			ImGui::PushID(ptr);

			auto saved_pos = ImGui::GetCursorPos();
			if (ImGui::InputTextMultiline("", &text, editing_rect.GetSize()))
			{
				auto pos_after_input = ImGui::GetCursorPos();
				ImGui::SetCursorPos(saved_pos);
				ImGui::TextColored(ImColor(0, 0, 0, 0), (text.empty() ? "Double-click to edit" : text.data()));
				
				ImGui::SetCursorPos(pos_after_input);

				ImRect new_rect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
				editing_rect = new_rect;
				if (!text.empty())
				{
					if (text.back() == '\n')
					{
						editing_rect.Expand(ImVec2(10, 16));
					}
					else
					{
						editing_rect.Expand(ImVec2(10, 16));
					}
				}
				
				

			}
			ImGui::PopID();
			return;
		}
		else
		{
			if (text.empty())
			{
				ImGui::Text("Double-click to edit");
			}
			else
			{
				ImGui::Text(text.data());
			}
			displaying_rect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

			if (!editing_rect_empty)
			{
				editing_rect = ImRect();
			}

			return;
		}
				
	}

	void RenderContent(Post& post, std::size_t content_idx, std::pair<ImRect, ImRect>& content_rects)
	{
		PostContent& content = post.content[content_idx];
		switch (content.GetType())
		{
		case(ContentType::text):
			RenderText(post, content_idx, content_rects);
			break;
		default:
			ImGui::Text("UNKNOWN CONTENT TYPE");
			break;
		}
	}

	void BoardTab::SetSelectedPost(std::size_t idx)
	{
		if (leftclicked_idx == idx) return;

		if (leftclicked_idx > 0) // "clear" the old selected node, if any
		{
			container[leftclicked_idx].editing_content = 0;
		}

		if (idx == 0)
		{
			leftclicked_idx = 0; 
			return;
		}
		std::size_t new_idx = container.MoveToLastPosition(idx)->GetIdx();
		leftclicked_idx = new_idx;
		current_frame.just_selected_post = true;
	}

	void StartEditingPost(Post& post, LuaVector<std::pair<ImRect, ImRect>>& content_pairs, ImVec2& mouse_pos)
	{
		auto& content = post.content;

		for (std::size_t i = 1; i <= content.size(); i++)
		{
			PostContent& c = content[i];
			auto& rect_pair = content_pairs[i];
			ImRect& curr_rect = (post.editing_content == i ? rect_pair.second : rect_pair.first);

			bool content_selected = curr_rect.Contains(mouse_pos);

			if (content_selected)
			{
			post.editing_content = i;
			break;
			}
		}
	}

	void BoardTab::RenderPost(Post& post)
	{
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		auto& display_pos = post.display_pos;
		std::size_t idx = post.GetIdx();

		ImRect& total_rect = posts_info[idx].total_rect;
		ImRect item_rect_outer = total_rect;
		item_rect_outer.Expand(5);

		ImGui::SetCursorPosX(display_pos.first);
		ImGui::SetCursorPosY(display_pos.second);
		
		bool has_info = total_rect.GetHeight() != 0;
		bool has_color = (post.color[0] >= 0 && post.color[1] >= 0 && post.color[2] >= 0);
		const auto& color = (has_color ? post.color : container.board_options.color_table.post_color);
		ImColor post_color_inner = ImColor(color[0], color[1], color[2]);
		ImColor post_color_outer = ImColor(color[0] * 0.70f, color[1] * 0.70f, color[2] * 0.70f);

		if (has_info)
		{
			draw_list->AddRectFilled(item_rect_outer.Min, item_rect_outer.Max, post_color_outer, 1.f);
			draw_list->AddRectFilled(total_rect.Min, total_rect.Max, post_color_inner, 1.f);
		}

		ImGui::BeginGroup();

		for (std::size_t i = 1; i <= post.content.size(); i++)
		{
			auto& content_rects = posts_info[idx].content_rects[i];
			if (i > 1) ImGui::NewLine();
			RenderContent(post, i, content_rects);			
		}

		ImGui::EndGroup();

		total_rect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
		total_rect.Expand(5);

		if (current_frame.mouse_clicked)
		{
			ImVec2& mouse_pos = current_frame.mouse_pos;
			bool inner_click = total_rect.Contains(mouse_pos);
			bool outer_click = item_rect_outer.Contains(mouse_pos);

			if (inner_click || outer_click)
			{
				if (current_frame.mouse_leftclicked)
				{
					current_frame.selection = idx;
				}
				else if (current_frame.mouse_rightclicked)
				{
					rightclicked_idx = idx;
				}				
			}
		}

		if (post.tags.HasTag("connects_to"))
		{
			auto& connections = post.tags["connects_to"];
			for (auto& entry : connections)
			{
				if (!entry.asInt()) continue;
				last_frame_info.connections.emplace_back(post.GetIdx(), entry.asInt());
			}
		}

		#ifdef BOARD_DEBUG
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetFontSize());
		ImGui::Text("%i", idx);
		#endif
		
	}

	void BoardTab::DragPost()
	{
		if (leftclicked_idx == 0) return;

		Post& current_post = container[leftclicked_idx];
		auto& post_display_pos = current_post.display_pos;

		if (current_post.editing_content != 0) return;

		current_frame.dragging_post = true;
		
		ImVec2 delta = ImGui::GetMouseDragDelta();
		post_display_pos.first += delta.x;
		post_display_pos.second += delta.y;
		ImGui::ResetMouseDragDelta();
	}

	void ShowColorPanel(utils::ColorTable& color_table)
	{
		ImGui::Begin("Color Panel", NULL, PopupWindowFlags);
		auto& post_color = color_table.post_color;
		auto& bg_color = color_table.bg_color;

		ImGui::PushID((void*)&post_color);
		ImGui::Text("Post Color:"); ImGui::SameLine();
		ImGui::ColorEdit3("", &post_color[0]);
		ImGui::PopID();

		ImGui::PushID((void*)&bg_color);
		ImGui::Text("Background Color:"); ImGui::SameLine();
		ImGui::ColorEdit3("", &bg_color[0]);
		ImGui::PopID();

		ImGui::End();
	}

	void BoardTab::ShowDebugWindow()
	{

		ShowColorPanel(container.board_options.color_table);

		ImVec2 content_max = ImGui::GetContentRegionMax();
		ImVec2 scroll_max = ImVec2(ImGui::GetScrollMaxX(), ImGui::GetScrollMaxY());
		ImVec2 curr_scroll = ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY());

		ImGui::Begin("Debug Window", NULL, PopupWindowFlags);

		ImGui::Text("Rendering context info");		
		ImGui::Text("Max: Right: %2.f, Down: %2.f", content_max.x, content_max.y);
		
		ImGui::NewLine();
		ImGui::Text("Last frame info");
		ImGui::Text("Content size: %2.f, %2.f", last_frame_info.content_size.x, last_frame_info.content_size.y);

		ImGui::NewLine();
		ImGui::Text("Scroll info:");
		ImGui::Text("Current: X: %2.f, Y: %2.f", curr_scroll.x, curr_scroll.y);
		ImGui::Text("Max: X: %2.f, Y: %2.f", scroll_max.x, scroll_max.y);


		if (ImGui::IsMousePosValid())
		{
			ImVec2 mouse_pos = ImGui::GetMousePos();
			ImGui::Text("Mouse Pos: %1.f, %1.f", mouse_pos.x, mouse_pos.y);
		}
		if (ImGui::IsMouseDragging(0))
		{
			ImGui::Text("Dragging Left Mouse Button");
			ImVec2 drag = ImGui::GetMouseDragDelta();
			ImGui::Text("Drag: %1.f, %1.f", drag.x, drag.y);
		}
		
		if (leftclicked_idx > 0 && current_frame.selection == 0)
		{
			const Post& selected_post = container[leftclicked_idx];
			const auto& content = selected_post.content;

			const PostRenderingInfo& post_info = posts_info[leftclicked_idx];
			const ImRect& total_rect = post_info.total_rect;
			const auto& content_rects = post_info.content_rects;

			ImGui::NewLine();
			ImGui::Text("Left clicked Post: %i", leftclicked_idx);
			ImGui::Text("Display Pos: %1.f, %1.f", selected_post.display_pos.first, selected_post.display_pos.second);
			ImGui::Text("Rect Min: %1.f, %1.f", total_rect.Min.x, total_rect.Min.y);
			ImGui::Text("Rect Max: %1.f, %1.f", total_rect.Max.x, total_rect.Max.y);
			ImGui::Text("Rect Size: %1.f, %1.f", total_rect.GetSize().x, total_rect.GetSize().y);
			
			for (std::size_t i = 1; i <= content.size(); i++)
			{
				const PostContent& c = content[i];
				const auto& rects = content_rects[i];
				const auto& displaying_rect = rects.first;
				const auto& editing_rect = rects.second;
				
				const auto& current_rect = (selected_post.editing_content == i ? editing_rect : displaying_rect);
				ImVec2 rect_size = current_rect.GetSize();

				ImGui::NewLine();
				ImGui::Text("Content %i: %s", i, c.GetPrettyType());
				ImGui::Text("Current mode: %s", (selected_post.editing_content == i ? "Editing" : "Displaying"));
				ImGui::Text("Rect Min: %1.f, %1.f", current_rect.Min.x, current_rect.Min.y);
				ImGui::Text("Rect Max: %1.f, %1.f", current_rect.Max.x, current_rect.Max.y);
				ImGui::Text("Rect Size: %1.f, %1.f", rect_size.x, rect_size.y);
			}

			if (current_frame.dragging_post)
			{
				ImGui::Text("Currently dragging post %i", selected_post.GetIdx());
			}
		}
		if (rightclicked_idx != 0)
		{
			ImGui::NewLine();
			ImGui::Text("Right clicked post: %i", rightclicked_idx);
		}

		ImGui::End();
	}

	void BoardTab::Render()
	{
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ColorTable::ArrayToImColor(container.board_options.color_table.bg_color).operator ImVec4());
		ImGui::BeginChild("active tab", ImVec2(0, 0), false, TabChildWindowFlags);

		float current_scroll_x = ImGui::GetScrollMaxX();
		float current_scroll_y = ImGui::GetScrollMaxY();

		if (current_scroll_x != last_frame_info.scroll_max_x) ImGui::SetScrollX(current_scroll_x);
		if (current_scroll_y != last_frame_info.scroll_max_y) ImGui::SetScrollY(current_scroll_y);

		current_frame.Reset();
		current_frame.mouse_leftclicked = ImGui::IsMouseClicked(0);
		current_frame.mouse_doubleclicked = ImGui::IsMouseDoubleClicked(0);
		current_frame.mouse_released = ImGui::IsMouseReleased(0);	
		current_frame.mouse_rightclicked = ImGui::IsMouseClicked(1);
		current_frame.mouse_clicked = current_frame.mouse_leftclicked || current_frame.mouse_rightclicked;
		
		if (current_frame.mouse_clicked)
		{
			current_frame.mouse_pos = ImGui::GetMousePos();
		}
		
		if (current_frame.dragging_post && ImGui::IsMouseReleased(0))
		{
			current_frame.dragging_post = false;
			current_frame.just_released_post = true;
		}

		if (ImGui::IsMouseDragging(0) && ImGui::IsWindowFocused())
		{
			DragPost();
		}

		for (auto& line : last_frame_info.connections)
		{
			std::size_t start_idx = line.first;
			std::size_t end_idx = line.second;
			ImVec2& start_p = posts_info[start_idx].total_rect.Max;
			ImVec2& end_p = posts_info[end_idx].total_rect.Min;

			ImGui::GetWindowDrawList()->AddLine(start_p, end_p, IM_COL32_WHITE);
		}

		last_frame_info.connections.clear();

		for (std::size_t i = 1; i <= container.size(); i++)
		{
			Post& post = container[i];
			RenderPost(post);	
		}

		#ifdef BOARD_DEBUG
		ShowDebugWindow();
		ImGui::ShowMetricsWindow();
		#endif

		if (current_frame.mouse_leftclicked)
		{
			SetSelectedPost(current_frame.selection);
		}

		if (current_frame.mouse_doubleclicked && leftclicked_idx > 0)
		{
			StartEditingPost(container[leftclicked_idx], posts_info[leftclicked_idx].content_rects, current_frame.mouse_pos);
		}

		if (current_frame.mouse_rightclicked)
		{
			if (rightclicked_idx == 0)
			{
				ImGui::OpenPopup("add post");
			}
			else
			{
				ImGui::OpenPopup("edit post");
			}
		}

		if (ImGui::BeginPopup("add post"))
		{
			const ImVec2 mouse_pos = ImGui::GetMousePosOnOpeningCurrentPopup();

			if (ImGui::MenuItem("Add Post"))
			{
				Post& new_post = *container.CreatePostBack();
				new_post.display_pos = { mouse_pos.x, mouse_pos.y };
			}
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopup("edit post"))
		{
			const ImVec2 mouse_pos = ImGui::GetMousePosOnOpeningCurrentPopup();
			Post& post = container[rightclicked_idx];

			if (ImGui::MenuItem("Edit Post"))
			{

			}
			if (ImGui::MenuItem("Remove Post"))
			{
				container.Erase(container.IteratorFromIndex(rightclicked_idx));
				rightclicked_idx = 0;
			}

			ImGui::EndPopup();
			
		}

		last_frame_info.scroll_max_x = ImGui::GetScrollMaxX();
		last_frame_info.scroll_max_y = ImGui::GetScrollMaxY();
		last_frame_info.content_size = ImGui::GetWindowContentRegionMax();

		ImGui::EndChild();
		ImGui::PopStyleColor();

	}
}