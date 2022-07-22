#pragma once

#include <vector>
#include <memory>

#include "Widget.hpp"
#include "renderables/tabs/TabBar.hpp"
#include "renderables/windows/Prompt.hpp"

namespace sb
{

	class WidgetManager
	{
		using PromptPointer = std::unique_ptr<Prompt>;
	public:
		void RenderAll();
		void NewErrorPrompt(const std::string& what);
		bool hasActiveTab() { return tab_bar.hasActiveTab(); }
		//TODO documentation that if !hasActiveTab() using the reference will be undefined behavior
		BoardTab& getActiveTab() { return tab_bar.getActiveTab(); }
	private:
		void CommandQueueLookup();

		TabBar tab_bar;

		std::vector<PromptPointer> errors;
		std::vector<std::vector<PromptPointer>::iterator> to_delete_errors;
		std::size_t curr_window_id = 0;

	};
}