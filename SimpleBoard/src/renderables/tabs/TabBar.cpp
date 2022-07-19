#include "TabBar.hpp"

#include <sstream>
#include <format>

#include "imgui.h"

#include "containers/PostContainer.hpp"
#include "utils/LuaStack.hpp"
#include "utils/parsing/BoardParser.hpp"
#include "renderables/DearImGuiFlags.hpp"
#include "utils/OperationsQueue.hpp"

using utils::LuaStack;
using utils::BoardParser;

namespace sb
{
    void TabBar::NewBoardTab(const std::string& path)
    {
        try
        {
            sol::table t = LuaStack::TableFromFile(path);
            PostContainer container = BoardParser().Parse(t);
            BoardTab new_tab(std::move(container), path);
            tabs.push_back(std::move(new_tab));
        }
        catch (const std::exception& e)
        {
            std::stringstream stream;
            stream << "Error creating new Tab: " << e.what() << '\n';
            OperationQueue::CreateErrorWindow(stream.str());
        }
    }
	void TabBar::NewBoardTab()
	{
        try
        {
            string unique_ID = std::format("New Board ({})", unnamed_tab_count++);
            tabs.emplace_back(std::move(unique_ID));
        }
        catch (const std::exception& e)
        {
            std::stringstream stream;
            stream << "Error creating new Tab: " << e.what() << '\n';
            OperationQueue::CreateErrorWindow(stream.str());
        }
	}

    void TabBar::Render()
    {
        if (ImGui::BeginTabBar("Main Tab Bar", TabBarFlags))
        {
            if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing))
            {
                NewBoardTab();
            }
            int count = -1;
            for (auto it = std::begin(tabs); it != std::end(tabs); it++)
            {
                count++;
                if (!(*it).is_open)
                {
                    to_delete.emplace_back(it);
                    continue;
                }
                auto& tab = (*it);
                ImGuiTabItemFlags set_selected = ImGuiTabItemFlags_None;
                if (change_to != -1 && change_to == count)
                {
                    set_selected |= ImGuiTabItemFlags_SetSelected;
                    change_to = -1;
                }
                if (ImGui::BeginTabItem(tab.GetDisplayName(), &tab.is_open, TabItemFlags | set_selected))
                { 
                    tab.Render();
                    ImGui::EndTabItem();

                    if (active_tab != count)
                    {
                        active_tab = count;
                    }
                }
            }

            if (!to_delete.empty())
            {
                for (int i = to_delete.size() - 1; i >= 0; i--)
                {
                    #ifdef BOARD_DEBUG
                    std::cout << "Disposing of window: '" << (*to_delete[i]).ID << "' \n";
                    #endif
                    tabs.erase(to_delete[i]);
                }
                to_delete.clear();
            }

            ImGui::EndTabBar();
        }
    }
}