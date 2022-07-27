#pragma once

#include "imgui.h"

namespace board
{
	const ImGuiWindowFlags ErrorPromptFlags =
		ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

	const ImGuiWindowFlags MainWindowFlags =
		ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoBringToFrontOnFocus;

	const ImGuiWindowFlags PopupWindowFlags =
		ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoCollapse;

	const ImGuiTabBarFlags TabBarFlags =
		ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_FittingPolicyScroll;

	const ImGuiTabItemFlags TabItemFlags =
		ImGuiTabItemFlags_None;

	const ImGuiWindowFlags TabChildWindowFlags =
		ImGuiWindowFlags_HorizontalScrollbar;

	const ImGuiTableFlags TableFlags =
		ImGuiTableFlags_Borders | ImGuiTableFlags_NoSavedSettings;

	const ImGuiTableFlags ResizableTableFlags =
		TableFlags | ImGuiTableFlags_Resizable;

	const ImGuiWindowFlags PostWindowFlags =
		ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
}