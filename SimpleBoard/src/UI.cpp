#include "sol/sol.hpp"

#include "imgui.h"

#include "UI.hpp"
#include "utils/OperationsQueue.hpp"
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
                                OperationQueue::addToQueue(OperationQueue::targets::widgetManager, OperationQueue::operations::openFile, path);
                            }
                        }
                    }

                }
                catch (const std::exception& e)
                {
                    widgets.NewErrorPrompt(e.what());
                }
            }
            if (!widgets.hasActiveTab())
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
            if (!widgets.hasActiveTab())
            {
                ImGui::EndDisabled();
            }
            if (ImGui::MenuItem("Test Errors"))
            {
                widgets.NewErrorPrompt("Didn't work.");
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Show all windows"))
            {

            }
            if (ImGui::MenuItem("Collapse all windows"))
            {

            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();

        try
        {
            widgets.RenderAll();
        }
        catch (std::exception& err)
        {
            std::cout << "Error: " << err.what();
        }

        #ifdef BOARD_DEBUG
        ImGui::ShowMetricsWindow();
        #endif
	}
}