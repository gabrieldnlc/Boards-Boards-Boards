#pragma once

#include "imgui.h"

namespace board
{
	static const ImGuiWindowFlags ErrorPromptFlags =
		ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

	static const ImGuiWindowFlags MainWindowFlags =
		ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoBringToFrontOnFocus;

	static const ImGuiWindowFlags PopupWindowFlags =
		ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoCollapse;

	static const ImGuiTabBarFlags TabBarFlags =
		ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_FittingPolicyScroll;

	static const ImGuiTabItemFlags TabItemFlags =
		ImGuiTabItemFlags_None;

	static const ImGuiWindowFlags TabChildWindowFlags =
		ImGuiWindowFlags_HorizontalScrollbar;

	static const ImGuiTableFlags TableFlags =
		ImGuiTableFlags_Borders | ImGuiTableFlags_NoSavedSettings;

	static const ImGuiTableFlags ResizableTableFlags =
		TableFlags | ImGuiTableFlags_Resizable;

	static const ImGuiWindowFlags PostWindowFlags =
		ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
}