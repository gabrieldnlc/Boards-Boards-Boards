#include "ErrorPrompt.hpp"

#ifdef BOARD_DEBUG
#include <iostream>
#endif

#include "imgui.h"


#include "renderables/DearImGuiFlags.hpp"

namespace sb
{
	void ErrorPrompt::Render()
	{
		if (first_config)
		{
			#ifdef BOARD_DEBUG
			std::cout << "Creating error prompt: " << ID << '\n';
			#endif
			auto& io = ImGui::GetIO();
			ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), 0, ImVec2(0.5f, 0.5f));
			//TODO not perfectly centered, in that viewport way

			first_config = false;
		}
		ImGui::Begin(ID.data(), &is_open, ErrorPromptFlags);
		ImGui::Text(what.data());
		ImVec2 simsize = ImGui::CalcTextSize("OK", NULL, false, 1.f); //TODO is this perfectly centered?
		float center = ImGui::GetWindowWidth() * 0.5f;
		ImGui::SetCursorPosX(center - simsize.x);
		if (ImGui::Button("OK"))
		{
			is_open = false;
		}
		ImGui::End();
	}
}