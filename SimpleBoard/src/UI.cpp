#include "sol/sol.hpp"

#include "imgui.h"

#include "UI.hpp"
#include "utils/CommandQueue.hpp"
#include "utils/LuaStack.hpp"
#include "utils/Error.hpp"
#include "utils/FileDialog.hpp"
#include "utils/parsing/BoardParser.hpp"

namespace sb
{
    using utils::LuaStack;
    using utils::BoardParser;

	void UI::Render()
	{
        ImGui::BeginMainMenuBar();

        const bool NoTab = !widgets.hasActiveTab();

        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open"))
            {
                using namespace sb;
                try
                {
                    std::vector<std::string> files = FileDialog::OpenMultiple();
                    if (!files.empty())
                    {
                        for (const std::string& path : files)
                        {
                            if (!path.empty())
                            {
                                CommandQueue::addToQueue(CommandQueue::targets::widgetManager, CommandQueue::commands::openFile, path);
                            }
                        }
                    }

                }
                catch (const std::exception& e)
                {
                    widgets.NewErrorPrompt(e.what());
                }
            }
            if (NoTab)
            {
                ImGui::BeginDisabled();
            }
            if (ImGui::MenuItem("Save"))
            {
                
                auto& tab = widgets.getActiveTab();
                std::string path = sb::FileDialog::Save();
                if (!path.empty())
                {
                    auto& container = tab.container;
                    sol::table t = BoardParser().ToTable(container);
                    try
                    {
                        LuaStack::TableToFile(t, path);
                        tab.path = path;
                        tab.current_status = BoardTab::status::fromdisk;
                    }
                    catch (const std::exception& e)
                    {
                        widgets.NewErrorPrompt(e.what());
                    }
                }
                
            }
            if (NoTab)
            {
                ImGui::EndDisabled();
            }
            #ifdef BOARD_DEBUG
            if (ImGui::MenuItem("Test Errors"))
            {
                widgets.NewErrorPrompt("Didn't work.");
            }
            #endif
            ImGui::EndMenu();
        }

        if (NoTab) ImGui::BeginDisabled();
        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Color Table"))
            {
                CommandQueue::addToQueue(CommandQueue::targets::currentTab, CommandQueue::commands::openBoardOptions);
            }
            ImGui::EndMenu();
        }
        if (NoTab) ImGui::EndDisabled();

        ImGui::EndMainMenuBar();

        try
        {
            widgets.RenderAll();
        }
        catch (std::exception& err)
        {
            std::cout << "Error: " << err.what() << '\n';
        }

        #ifdef BOARD_DEBUG
        ImGui::ShowMetricsWindow();
        #endif
	}
}