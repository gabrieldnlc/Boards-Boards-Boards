#pragma once

#include <string>

#include "imgui_internal.h" //for ImRect

#include "renderables/Widget.hpp"
#include "renderables/posts/Post.hpp"
#include "containers/PostContainer.hpp"
#include "utils/FilePath.hpp"

namespace sb
{
    using std::string;

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
            
            bool just_selected_post = false;
            bool just_released_post = false;

            struct Mouse
            {
                bool valid = false;
                bool clicked = false;
                bool leftclicked = false;
                bool rightclicked = false;
                bool doubleclicked = false;
                bool released = false;
                bool dragging_leftclick = false;
                bool dragging_post = false;
                ImVec2 pos = ImVec2();

                void Reset()
                {
                    released = leftclicked = doubleclicked =
                        valid = rightclicked = clicked = false;
                    pos = ImVec2();
                }
            }mouse;

            struct Selections
            {
                std::size_t connection = 0;
                std::size_t post = 0;

            }selections;

            struct NewConnection
            {
                bool creating = false;
                std::size_t from = 0;
                std::size_t to = 0;

                void Reset()
                {
                    creating = false;
                    from = to = 0;
                }
            }new_connection;

            void Reset()
            {
                mouse.Reset();
                just_released_post = just_selected_post = false;
                selections.post = 0;
            }
        }curr_frame;

        
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
