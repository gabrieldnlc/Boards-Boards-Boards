#pragma once

#include <vector>
#include <memory>

#include "renderables/Widget.hpp"
#include "renderables/tabs/BoardTab.hpp"

namespace board
{
	class TabBar : public Widget
	{

	friend class WidgetManager;

	public:
		TabBar() : Widget("Tab Bar") {}
		void Render() override;
	private:
		void NewBoardTab();

		void NewBoardTab(const std::string& path);
		

		std::vector<BoardTab> tabs;
		using BoardIt = std::vector<BoardTab>::iterator;
		std::vector<BoardIt> to_delete;

		
		bool hasActiveTab() { return active_tab >= 0; }

		// - Will throw if !hasActiveTab()
		BoardTab& getActiveTab()
		{
			return (*(std::next(std::begin(tabs), active_tab)));
		}

		int active_tab = -1;
		int change_to = -1;

		std::size_t unnamed_tab_count = 1;

	};
}

