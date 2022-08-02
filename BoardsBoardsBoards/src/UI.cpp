#include <future>

#include "sol/sol.hpp"
#include "imgui.h"
#include "UI.hpp"
#include "utils/CommandQueue.hpp"
#include "utils/LuaStack.hpp"
#include "utils/Error.hpp"
#include "utils/FileDialog.hpp"
#include "utils/parsing/BoardParser.hpp"

namespace board
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
                const auto open = [this]()
                {
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
                        this->widgets.NewErrorPrompt(e.what());
                    }
                };

                auto fut = std::async(std::launch::async, open);
                
            }
            if (NoTab)
            {
                ImGui::BeginDisabled();
            }
            if (ImGui::MenuItem("Save"))
            {
                
                auto& tab = widgets.getActiveTab();
                std::string path = board::FileDialog::Save();
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
            if (ImGui::MenuItem("Board options"))
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