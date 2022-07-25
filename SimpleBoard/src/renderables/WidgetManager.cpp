#include "WidgetManager.hpp"

#ifdef BOARD_DEBUG
#include <iostream>
#endif

#include "imgui.h"

#include "utils/Error.hpp"
#include "utils/CommandQueue.hpp"
#include "renderables/windows/ErrorPrompt.hpp"
#include "renderables/DearImGuiFlags.hpp"


namespace sb
{

	void WidgetManager::CommandQueueLookup()
	{
		while (!CommandQueue::empty(CommandQueue::targets::widgetManager))
		{
			auto& pair = CommandQueue::front(CommandQueue::targets::widgetManager);
			auto& command = pair.first;
			const std::string& data = pair.second;
			switch (command)
			{
			case (CommandQueue::commands::createErrorWindow):
				NewErrorPrompt(data);
				break;
			case (CommandQueue::commands::openFile):
				try
				{
					tab_bar.NewBoardTab(data);
				}
				catch (const std::exception& e)
				{
					NewErrorPrompt(e.what());
				}
				break;
			default:
				throw utils::CommandQueueError("Widget Manager received unknown command.");
			}
			CommandQueue::pop(CommandQueue::targets::widgetManager);
		}
	}

	void WidgetManager::NewErrorPrompt(const std::string& what) 
	{
		curr_window_id++;
		ImGui::SetNextWindowFocus();
		errors.emplace_back(std::make_unique<ErrorPrompt>(what, "Error##" + std::to_string(curr_window_id)));
		#ifdef BOARD_DEBUG
		std::cout << "Current Windows ID: " << curr_window_id << '\n';
		#endif	
	}

	void WidgetManager::RenderAll()
	{
		ImVec2 size = ImGui::GetMainViewport()->WorkSize;
		ImVec2 pos = ImGui::GetMainViewport()->WorkPos;
		ImGui::SetNextWindowSize(size);
		ImGui::SetNextWindowPos(pos);
		ImGui::Begin("Main Window", NULL, MainWindowFlags);

		tab_bar.Render();

		ImGui::End();

		CommandQueueLookup();

		for (auto it = std::begin(errors); it != std::end(errors); it++)
		{
			if (!(*it)->is_open)
			{
				to_delete_errors.emplace_back(it);
				continue;
			}
			(*it)->Render();
		}

		if (!to_delete_errors.empty())
		{
			for (int i = to_delete_errors.size() - 1; i >= 0; i--)
			{
				#ifdef BOARD_DEBUG
				std::cout << "Disposing of window: '" << (*to_delete_errors[i])->ID << "' \n";
				#endif
				errors.erase(to_delete_errors[i]);
			}
			to_delete_errors.clear();
		}	
}
}