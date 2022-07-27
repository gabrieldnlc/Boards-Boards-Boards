#include "BoardTab.hpp"

#include "imgui.h"
#include "imgui_stdlib.h"

#include "renderables/DearImGuiFlags.hpp"
#include "renderables/posts/PostContent.hpp"
#include "utils/BoardColors.hpp"
#include "utils/Bezier.hpp"
#include "utils/CommandQueue.hpp"
#include "utils/Error.hpp"

#include <iostream>

using utils::BoardColors;
using utils::CubicBezier;
using utils::GetCubicBezier;
using utils::DrawCubicBezier;
using utils::IsMouseOnCubicBezier;

namespace board
{
	BoardTab::BoardTab(string unique_ID) : Widget(unique_ID), path(std::move(unique_ID)), posts_info(true) {}
	BoardTab::BoardTab(PostContainer&& container, string path) : container(std::move(container)), Widget(path), path(std::move(path)), posts_info(true) {}
	BoardTab::BoardTab(const PostContainer& container, string path) : container(container), Widget(path), path(std::move(path)), posts_info(true) {}

	
	void RenderText(Post& post, std::size_t content_idx, std::pair<ImRect, ImRect>& content_rects, BoardColors& colors, float s_unit)
	{
		PostContent& content = post.content[content_idx];

		const std::string empty_post = "Double click here";
		auto& text = content.AsString();
		auto& string = (text.empty() ? empty_post : text);
		
		const bool is_editing = post.editing_content == content_idx;
		auto& bg_color = (post.HasColor() ? post.color : colors.post);
		auto& text_color = (is_editing ? bg_color : colors.text);

		auto pos = ImGui::GetCursorPos();

		ImGui::TextColored(colors.ArrayToImColor(text_color), string.data());

		ImRect text_rect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
		content_rects.first = text_rect;

		if (!is_editing) return;

		ImGui::SetCursorPos(pos);

		ImRect edit_rect = text_rect;
		edit_rect.Expand(s_unit * 2);
		
		ImGui::PushID((void*)&content);
		ImGui::InputTextMultiline("", &text, edit_rect.GetSize());
		ImGui::PopID();

		content_rects.second = edit_rect;
	}
	
	void RenderContent(Post& post, std::size_t content_idx, std::pair<ImRect, ImRect>& content_rects, BoardColors& colors, float s_unit)
	{
		PostContent& content = post.content[content_idx];
		switch (content.GetType())
		{
		case(ContentType::text):
			RenderText(post, content_idx, content_rects, colors, s_unit);
			break;
		default:
			ImGui::Text("UNKNOWN CONTENT TYPE");
			break;
		}
	}

	void StartEditingPost(Post& post, LuaVector<std::pair<ImRect, ImRect>>& content_pairs, ImVec2& mouse_pos)
	{
		auto& content = post.content;

		for (std::size_t i = 1; i <= content.size(); i++)
		{
			PostContent& c = content[i];
			auto& rect_pair = content_pairs[i];
			ImRect& curr_rect = (post.editing_content == i ? rect_pair.second : rect_pair.first);

			const bool content_selected = curr_rect.Contains(mouse_pos);

			if (content_selected)
			{
				post.editing_content = i;
				break;
			}
		}
	}

	void ShowBoardOptions(PostContainer::BoardOptions& board_options, bool* is_open = NULL)
	{
		auto& color_table = board_options.color_table;

		float y_pos[5];
		float x_pos = 0;

		ImGui::Begin("Color Panel", is_open, PopupWindowFlags);
		auto& post = color_table.post;
		auto& bg = color_table.bg;
		auto& connection = color_table.connection;
		auto& selected_connection = color_table.selected_connection;
		auto& text = color_table.text;

		
		ImGui::Text("Posts:"); ImGui::SameLine();
		if (ImGui::GetCursorPosX() > x_pos) x_pos = ImGui::GetCursorPosX();
		y_pos[0] = ImGui::GetCursorPosY();
		ImGui::NewLine();
		ImGui::NewLine();

		
		ImGui::Text("Background:"); ImGui::SameLine();
		if (ImGui::GetCursorPosX() > x_pos) x_pos = ImGui::GetCursorPosX();
		y_pos[1] = ImGui::GetCursorPosY();
		ImGui::NewLine();
		ImGui::NewLine();

		
		ImGui::Text("Connections:"); ImGui::SameLine();
		if (ImGui::GetCursorPosX() > x_pos) x_pos = ImGui::GetCursorPosX();
		y_pos[2] = ImGui::GetCursorPosY();
		ImGui::NewLine();
		ImGui::NewLine();


		ImGui::Text("Selected connection:"); ImGui::SameLine();
		if (ImGui::GetCursorPosX() > x_pos) x_pos = ImGui::GetCursorPosX();
		y_pos[3] = ImGui::GetCursorPosY();
		ImGui::NewLine();
		ImGui::NewLine();


		ImGui::Text("Text:"); ImGui::SameLine();
		if (ImGui::GetCursorPosX() > x_pos) x_pos = ImGui::GetCursorPosX();
		y_pos[4] = ImGui::GetCursorPosY();
		ImGui::NewLine();
		ImGui::NewLine();

		ImGui::PushID((void*)&post);
		ImGui::SetCursorPos(ImVec2(x_pos, y_pos[0]));
		ImGui::ColorEdit3("", &post[0]);
		ImGui::PopID();

		ImGui::PushID((void*)&bg);
		ImGui::SetCursorPos(ImVec2(x_pos, y_pos[1]));
		ImGui::ColorEdit3("", &bg[0]);
		ImGui::PopID();

		ImGui::PushID((void*)&connection);
		ImGui::SetCursorPos(ImVec2(x_pos, y_pos[2]));
		ImGui::ColorEdit3("", &connection[0]);
		ImGui::PopID();

		ImGui::PushID((void*)&selected_connection);
		ImGui::SetCursorPos(ImVec2(x_pos, y_pos[3]));
		ImGui::ColorEdit3("", &selected_connection[0]);
		ImGui::PopID();

		ImGui::PushID((void*)&text);
		ImGui::SetCursorPos(ImVec2(x_pos, y_pos[4]));
		ImGui::ColorEdit3("", &text[0]);
		ImGui::PopID();

		ImGui::End();
	}

	void AlignButtonsRight(std::size_t num_buttons, float button_width)
	{
		const float offset = 10.f;
		float avail_x = ImGui::GetWindowContentRegionMax().x;

		float pos;
		if (num_buttons == 1)
		{
			pos = (avail_x - button_width);
		}
		else
		{
			pos = avail_x;
			pos -= (button_width * num_buttons);
			pos -= (ImGui::GetStyle().ItemSpacing.x * num_buttons);
		}
		ImGui::SetCursorPosX(pos);
	}

	void BoardTab::CommandQueueLookup()
	{
		while (!CommandQueue::empty(CommandQueue::targets::currentTab))
		{
			auto& pair = CommandQueue::front(CommandQueue::targets::currentTab);
			auto& command = pair.first;
			const std::string& data = pair.second;

			switch (command)
			{
			case (CommandQueue::commands::openBoardOptions):
				curr_frame.popups.table_options.open = true;
				break;
			default:
				throw utils::CommandQueueError("Current Tab received unknown command.");
			}

			CommandQueue::pop(CommandQueue::targets::currentTab);
			
		}
	}

	void BoardTab::SetSelectedPost(std::size_t idx)
	{
		if (curr_frame.selections.leftclicked == idx) return;

		if (curr_frame.selections.leftclicked > 0) // "clear" the old selected node, if any
		{
			container[curr_frame.selections.leftclicked].editing_content = 0;
		}

		if (idx == 0)
		{
			curr_frame.selections.leftclicked = 0;
			return;
		}
		std::size_t new_idx = container.MoveToLastPosition(idx)->GetIdx();
		curr_frame.selections.leftclicked = new_idx;
		curr_frame.just_selected_post = true;

		curr_frame.popups.editing_post.WillRefresh(true);
	}
	
	void BoardTab::RenderPost(Post& post)
	{
		BoardColors& color_table = container.board_options.color_table;

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		auto& display_pos = post.display_pos;
		std::size_t idx = post.GetIdx();

		ImRect& total_rect = posts_info[idx].total_rect;
		ImRect item_rect_outer = total_rect;
		item_rect_outer.Expand(s_unit * 0.5f);

		ImGui::SetCursorPosX(display_pos.first);
		ImGui::SetCursorPosY(display_pos.second);
		
		const bool has_info = total_rect.GetHeight() != 0;
		const bool has_color = (post.color[0] >= 0 && post.color[1] >= 0 && post.color[2] >= 0);

		const auto& color = (has_color ? post.color : color_table.post);

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
			RenderContent(post, i, content_rects, color_table, s_unit);			
		}

		ImGui::EndGroup();

		total_rect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
		total_rect.Expand(s_unit * 0.5f);

		ImVec2& mouse_pos = curr_frame.mouse.pos;
		const bool mouse_inner = total_rect.Contains(mouse_pos);
		const bool mouse_outer = item_rect_outer.Contains(mouse_pos);

			if (mouse_inner || mouse_outer)
			{
				curr_frame.hovering.post = idx;
				
				if (curr_frame.mouse.leftclicked)
				{
					curr_frame.just_selected_post = idx;
				}
				else if (curr_frame.mouse.rightclicked)
				{
					curr_frame.selections.rightclicked = idx;
				}				
			}
		

		#ifdef BOARD_DEBUG
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetFontSize());
		ImGui::Text("%i", idx);
		#endif
		
	}

	ImVec2 GetRectCenter(const ImRect& rect)
	{
		ImVec2 size = rect.GetSize();
		float middle_x = rect.Min.x + (size.x / 2);
		float middle_y = rect.Min.y + (size.y / 2);
		return ImVec2(middle_x, middle_y);
	}

	void BoardTab::DragSelectedPost()
	{
		if (curr_frame.selections.leftclicked == 0) return;

		Post& current_post = container[curr_frame.selections.leftclicked];
		auto& post_display_pos = current_post.display_pos;

		if (current_post.editing_content != 0) return;

		curr_frame.mouse.dragging_post = true;
		
		ImVec2 delta = ImGui::GetMouseDragDelta();
		post_display_pos.first += delta.x;
		post_display_pos.second += delta.y;
		ImGui::ResetMouseDragDelta();
	}

	void BoardTab::ShowDebugWindow()
	{

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


		if (curr_frame.mouse.valid)
		{
			ImGui::Text("Mouse Pos: %1.f, %1.f", curr_frame.mouse.pos.x, curr_frame.mouse.pos.y);
		}
		if (ImGui::IsMouseDragging(0))
		{
			ImGui::Text("Dragging Left Mouse Button");
			ImVec2 drag = ImGui::GetMouseDragDelta();
			ImGui::Text("Drag: %1.f, %1.f", drag.x, drag.y);
		}
		
		if (curr_frame.selections.leftclicked > 0)
		{
			const Post& post = container[curr_frame.selections.leftclicked];
			const auto& content = post.content;

			const PostRenderingInfo& post_info = posts_info[curr_frame.selections.leftclicked];
			const ImRect& total_rect = post_info.total_rect;
			const auto& content_rects = post_info.content_rects;

			ImGui::NewLine();
			ImGui::Text("Left clicked Post: %i", curr_frame.selections.leftclicked);
			ImGui::Text("Display Pos: %1.f, %1.f", post.display_pos.first, post.display_pos.second);
			ImGui::Text("Rect Min: %1.f, %1.f", total_rect.Min.x, total_rect.Min.y);
			ImGui::Text("Rect Max: %1.f, %1.f", total_rect.Max.x, total_rect.Max.y);
			ImGui::Text("Rect Size: %1.f, %1.f", total_rect.GetSize().x, total_rect.GetSize().y);
			
			for (std::size_t i = 1; i <= content.size(); i++)
			{
				const PostContent& c = content[i];
				const auto& rects = content_rects[i];
				const auto& displaying_rect = rects.first;
				const auto& editing_rect = rects.second;
				
				const auto& current_rect = (post.editing_content == i ? editing_rect : displaying_rect);
				ImVec2 rect_size = current_rect.GetSize();

				ImGui::NewLine();
				ImGui::Text("Content %i: %s", i, c.GetPrettyType());
				ImGui::Text("Current mode: %s", (post.editing_content == i ? "Editing" : "Displaying"));
				ImGui::Text("Rect Min: %1.f, %1.f", current_rect.Min.x, current_rect.Min.y);
				ImGui::Text("Rect Max: %1.f, %1.f", current_rect.Max.x, current_rect.Max.y);
				ImGui::Text("Rect Size: %1.f, %1.f", rect_size.x, rect_size.y);
			}

			if (curr_frame.mouse.dragging_post)
			{
				ImGui::Text("Currently dragging post %i", post.GetIdx());
			}
		}
		if (curr_frame.selections.rightclicked != 0)
		{
			ImGui::NewLine();
			ImGui::Text("Right clicked post: %i", curr_frame.selections.rightclicked);
		}
		if (curr_frame.hovering.connection > 0)
		{
			ImGui::Text("Hovered connection: %i", curr_frame.hovering.connection);
		}
		if (curr_frame.hovering.post > 0)
		{
			ImGui::Text("Hovered post: %i", curr_frame.hovering.post);
		}

		ImGui::End();
	}

	void BoardTab::PopulateCurrentFrameInfo()
	{
		curr_frame.Reset();
		curr_frame.mouse.valid = ImGui::IsMousePosValid();
		curr_frame.mouse.leftclicked = ImGui::IsMouseClicked(0);
		curr_frame.mouse.doubleclicked = ImGui::IsMouseDoubleClicked(0);
		curr_frame.mouse.released = ImGui::IsMouseReleased(0);
		curr_frame.mouse.rightclicked = ImGui::IsMouseClicked(1);
		curr_frame.mouse.clicked = curr_frame.mouse.leftclicked || curr_frame.mouse.rightclicked;
		curr_frame.mouse.pos = ImGui::GetMousePos();
	}

	void BoardTab::RenderConnections()
	{
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		BoardColors& colors = container.board_options.color_table;

		std::size_t hovered = 0;

		const float connection_thickness = s_unit / 3;
		const float selection_threshold = s_unit / 2;
		const float selected_connection_thickness = 0.8f * s_unit;

		auto& DrawConnection = DrawCubicBezier;

		for (std::size_t i = 1; i <= container.connections.size(); i++)
		{
			auto& connection = container.connections[i];

			draw_list->ChannelsSetCurrent(1);

			std::size_t start_idx = connection.from;
			std::size_t end_idx = connection.to;
			
			ImVec2 start_p = GetRectCenter(posts_info[start_idx].total_rect);
			ImVec2 end_p = GetRectCenter(posts_info[end_idx].total_rect);

			CubicBezier bezier = GetCubicBezier(start_p, end_p);

			DrawCubicBezier(bezier, colors.ArrayToImColor(colors.connection), connection_thickness);

			if (IsMouseOnCubicBezier(bezier, curr_frame.mouse.pos, selection_threshold))
			{
				hovered = i;
			}
		}

		if (hovered > 0 && curr_frame.hovering.connection == 0)
		{
			curr_frame.hovering.connection = hovered;
		}

		if (curr_frame.hovering.connection > 0)
		{
			auto& connection = container.connections[curr_frame.hovering.connection];

			std::size_t start_idx = connection.from;
			std::size_t end_idx = connection.to;

			ImVec2 start_p = GetRectCenter(posts_info[start_idx].total_rect);
			ImVec2 end_p = GetRectCenter(posts_info[end_idx].total_rect);

			draw_list->ChannelsSetCurrent(0);

			CubicBezier bezier = GetCubicBezier(start_p, end_p);

			DrawCubicBezier(bezier, colors.ArrayToImColor(colors.selected_connection), selected_connection_thickness);
		}

		if (curr_frame.new_connection.creating)
		{
			auto& post_rect = posts_info[curr_frame.new_connection.from].total_rect;
			ImVec2 from = GetRectCenter(post_rect);
			ImVec2 to = curr_frame.mouse.pos;

			draw_list->ChannelsSetCurrent(1);

			CubicBezier bezier = GetCubicBezier(from, to);

			DrawCubicBezier(bezier, colors.ArrayToImColor(colors.connection), connection_thickness);

		}
	}

	void BoardTab::Render()
	{
		CommandQueueLookup();

		BoardColors& color_table = container.board_options.color_table;

		ImGui::PushStyleColor(ImGuiCol_ChildBg, BoardColors::ArrayToImColor(color_table.bg).operator ImVec4());
		
		ImGui::BeginChild("active tab", ImVec2(0, 0), false, TabChildWindowFlags);
		
		s_unit = ImGui::GetFontSize();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		const float current_scroll_x = ImGui::GetScrollMaxX();
		const float current_scroll_y = ImGui::GetScrollMaxY();

		if (current_scroll_x != last_frame_info.scroll_max_x) ImGui::SetScrollX(current_scroll_x);
		if (current_scroll_y != last_frame_info.scroll_max_y) ImGui::SetScrollY(current_scroll_y);

		PopulateCurrentFrameInfo();
		
		if (curr_frame.mouse.dragging_post && curr_frame.mouse.released)
		{
			curr_frame.mouse.dragging_post = false;
			curr_frame.just_released_post = true;
		}

		if (ImGui::IsMouseDragging(0) && ImGui::IsWindowFocused())
		{
			DragSelectedPost();
		}

		draw_list->ChannelsSplit(3);
		/*
			Ch 0 -> Reserved for indicating the hovered over connection, if any
			Ch 1 -> Connections (bezier curves)
			Ch 2 -> Posts
		*/
		draw_list->ChannelsSetCurrent(2);

		for (std::size_t i = 1; i <= container.size(); i++)
		{
			Post& post = container[i];
			RenderPost(post);
		}

		RenderConnections();

		draw_list->ChannelsMerge();

		#ifdef BOARD_DEBUG
		ShowDebugWindow();
		ImGui::ShowMetricsWindow();
		#endif

		if (curr_frame.mouse.leftclicked && ImGui::IsWindowHovered())
		{
			if (!ImGui::IsPopupOpen("right click on post"))
			{
				curr_frame.selections.rightclicked = 0;
			}

			if (curr_frame.new_connection.creating)
			{
				if (curr_frame.hovering.post == 0 || curr_frame.new_connection.from == curr_frame.hovering.post)
				{
					curr_frame.new_connection.Reset();
				}
				else
				{
					curr_frame.new_connection.to = curr_frame.hovering.post;

					container.connections.EmplaceBack(curr_frame.new_connection.from, curr_frame.new_connection.to);

					curr_frame.new_connection.Reset();
				}
			}
			else
			{
				SetSelectedPost(curr_frame.just_selected_post);
				
			}
			
		}

		if (curr_frame.mouse.doubleclicked && curr_frame.selections.leftclicked > 0)
		{
			StartEditingPost(container[curr_frame.selections.leftclicked], posts_info[curr_frame.selections.leftclicked].content_rects, curr_frame.mouse.pos);
		}

		if (curr_frame.mouse.rightclicked)
		{
			if (curr_frame.new_connection.creating)
			{
				curr_frame.new_connection.Reset();
			}
			if (curr_frame.hovering.post > 0)
			{
				if (container[curr_frame.hovering.post].editing_content == 0)
				{
					curr_frame.selections.rightclicked = curr_frame.hovering.post;
					ImGui::OpenPopup("right click on post");
					curr_frame.popups.editing_post.SetOpen(false);
				}
				
			}
			else if (curr_frame.hovering.connection > 0)
			{
				ImGui::OpenPopup("right click on connection");
			}
			else
			{
				ImGui::OpenPopup("right click on blank");
			}
		}

		if (ImGui::BeginPopup("right click on connection"))
		{
			if (ImGui::MenuItem("Remove connection"))
			{
				auto to_remove = container.connections.IteratorFromIndex(curr_frame.hovering.connection);
				container.connections.Erase(to_remove);
				curr_frame.hovering.connection = 0;
			}
			ImGui::EndPopup();
		}
		else
		{
			curr_frame.hovering.connection = 0;
		}

		if (ImGui::BeginPopup("right click on blank"))
		{
			const ImVec2 mouse_pos = ImGui::GetMousePosOnOpeningCurrentPopup();

			if (ImGui::MenuItem("Add Post"))
			{
				Post& new_post = *container.CreatePostBack();
				new_post.display_pos = { mouse_pos.x, mouse_pos.y };
				curr_frame.selections.leftclicked = 0;
			}
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopup("right click on post"))
		{
			const ImVec2 mouse_pos = ImGui::GetMousePosOnOpeningCurrentPopup();
			Post& post = container[curr_frame.selections.rightclicked];

			if (ImGui::MenuItem("Edit Post"))
			{
				curr_frame.popups.editing_post.SetOpen(true);
			}

			if (ImGui::MenuItem("Remove Post"))
			{// TODO: confirmation of deletion
				container.Erase(container.IteratorFromIndex(curr_frame.selections.rightclicked));
				curr_frame.selections.rightclicked = 0;
				curr_frame.selections.leftclicked = 0;
			}
			if (ImGui::MenuItem("Connect to..."))
			{
				curr_frame.new_connection.creating = true;
				curr_frame.new_connection.from = curr_frame.selections.rightclicked;
			}

			ImGui::EndPopup();
			
		}
		else
		{
			//curr_frame.selections.rightclicked = 0;
		}

		if (curr_frame.popups.editing_post.Open())
		{
			auto& vars = curr_frame.popups.editing_post;
			
			bool open = true;

			if (curr_frame.selections.rightclicked == 0)
			{
				vars.SetOpen(false);
			}
			else
			{
				Post& post = container[curr_frame.selections.rightclicked];

				bool default_color = !post.HasColor();

				if (vars.NeedsRefresh())
				{
					vars.Refresh(post);
				}

				ImGui::Begin("Edit post", &open, PopupWindowFlags);

				ImGui::Text("Content: ");
				ImGui::SameLine();

				ImGui::BeginGroup();

				for (auto& content : post.content)
				{
					switch (content.GetType())
					{
					case ContentType::text:
						ImGui::PushID((void*)&content.AsString());

						ImGui::InputTextMultiline("", &content.AsString());

						ImGui::PopID();
						break;
					default:
						ImGui::Text("Image support is WIP.");
					}
				}

				ImGui::EndGroup();

				ImGui::NewLine();

				if (ImGui::Checkbox("Use default color", &default_color))
				{
					if (!default_color)
					{
						post.color[0] = color_table.RGBIntToFloat(84);
						post.color[1] = color_table.RGBIntToFloat(84);
						post.color[2] = color_table.RGBIntToFloat(84);
					}
					else
					{
						post.color[0] = -1.f;
						post.color[1] = -1.f;
						post.color[2] = -1.f;
					}
				}

				if (default_color)
				{
					ImGui::BeginDisabled();
				}
				ImGui::BeginGroup();

				ImGui::PushID((void*)&post.color);

				ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview;
				if (default_color)
				{
					flags |= ImGuiColorEditFlags_NoInputs;
				}
				else
				{
					flags |= ImGuiColorEditFlags_DisplayRGB;
				}

				ImGui::ColorPicker3("", post.color, flags);
				ImGui::PopID();


				/*  Palette colors  */

				const int colors_per_row = 8;
				const ImVec2 color_size = ImVec2(s_unit * 1.5f, s_unit * 1.5f);
				const auto& palette = color_table.palette;

				for (int n = 0; n < IM_ARRAYSIZE(palette); n++)
				{
					ImGui::PushID(n);
					if ((n % colors_per_row) != 0)
					{
						ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.y);
					}
						
					const ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip;
					if (ImGui::ColorButton("##palette", palette[n], flags, color_size))
					{
						post.color[0] = palette[n].x;
						post.color[1] = palette[n].y;
						post.color[2] = palette[n].z;
					}
					ImGui::PopID();
				}

				/*  Palette colors  */
				
				ImGui::EndGroup();

				if (default_color)
				{
					ImGui::EndDisabled();
				}

				ImGui::Separator();

				const ImVec2 cancel_size = ImGui::CalcTextSize("Cancel") * 1.5;

				AlignButtonsRight(2, cancel_size.x);

				if (ImGui::Button("Cancel", cancel_size))
				{
					post = vars.CachedPost();
					vars.SetOpen(false);
				}
				ImGui::SameLine();

				if (ImGui::Button("OK", cancel_size))
				{
					vars.SetOpen(false);
				}

				if (!open)
				{
					post = vars.CachedPost();
					vars.SetOpen(false); 
				}

				ImGui::End();
			}

		}

		if (curr_frame.popups.table_options.open)
		{
			ShowBoardOptions(container.board_options, &curr_frame.popups.table_options.open);
		}

		last_frame_info.scroll_max_x = ImGui::GetScrollMaxX();
		last_frame_info.scroll_max_y = ImGui::GetScrollMaxY();
		last_frame_info.content_size = ImGui::GetWindowContentRegionMax();

		ImGui::EndChild();
		ImGui::PopStyleColor();

	}
}