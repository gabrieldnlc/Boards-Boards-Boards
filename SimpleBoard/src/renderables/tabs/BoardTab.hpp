#pragma once

#include "imgui_internal.h" //for ImRect

#include "renderables/Widget.hpp"
#include "renderables/posts/Post.hpp"
#include "containers/PostContainer.hpp"
#include "utils/FilePath.hpp"
#include "utils/ColorTable.hpp"

namespace sb
{
    using std::string;
    using utils::ColorTable;

    class BoardTab : public Widget
    {
    public:
        BoardTab(string unique_ID);
        BoardTab(PostContainer&& container, string path);
        BoardTab(const PostContainer& container, string path);
        void Render();
        enum class status { unnamed_file, fromdisk, fromdisk_modified };
        status current_status = status::unnamed_file;
        bool is_open = true;
        const char* GetDisplayName() { return path.data(); }
        PostContainer container;
        FilePath path;
    private:

        struct CurrentFrameInfo
        {
            bool mouse_valid = false;
            bool mouse_clicked = false;
            bool mouse_leftclicked = false;
            bool mouse_doubleclicked = false;
            bool mouse_released = false;
            bool dragging_leftclick = false;
            bool dragging_post = false;
            bool mouse_rightclicked = false;
            bool just_selected_post = false;
            bool just_released_post = false;

            std::size_t selected_post = 0;
            std::size_t hovered_connection = 0;

            ImVec2 mouse_pos = ImVec2();

            void Reset()
            {
                mouse_released = mouse_leftclicked = mouse_doubleclicked = mouse_valid =
                mouse_rightclicked = mouse_clicked = just_released_post = just_selected_post = false;
                selected_post = 0;
                mouse_pos = ImVec2();
            }
        }current_frame;

        
        void PopulateCurrentFrameInfo();
        void RenderPost(Post& post);
        void RenderConnections();
        void ShowDebugWindow();
        
        
        float s_unit; // Short for "screen unit" -> ImGui::GetFontSize() every frame

        

        std::size_t leftclicked_idx = 0;
        std::size_t rightclicked_idx = 0;

        void SetSelectedPost(std::size_t idx);
        void DragSelectedPost();

        struct PostRenderingInfo
        {
            ImRect total_rect;
            
            // pair.first = displaying rectangle
            // pair.second = editing rectangle
            LuaVector<std::pair<ImRect, ImRect>> content_rects = LuaVector<std::pair<ImRect, ImRect>>(true);
        };
        LuaVector<PostRenderingInfo> posts_info;

        struct LastFrameInfo
        {
            float scroll_max_x = 0;
            float scroll_max_y = 0;

            ImVec2 content_size = ImVec2();
            void Reset() 
            {
                scroll_max_x = scroll_max_y = 0; 
            }

        }last_frame_info;      
    };
}
