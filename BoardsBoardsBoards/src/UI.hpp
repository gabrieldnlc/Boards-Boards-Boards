#pragma once

#include "renderables/WidgetManager.hpp"

namespace board
{
	class UI
	{
	public:
		void Render();
	private:
		WidgetManager widgets;
	};
}