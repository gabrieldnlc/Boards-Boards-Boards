#pragma once

#include "renderables/WidgetManager.hpp"

namespace sb
{
	class UI
	{
	public:
		void Render();
	private:
		WidgetManager widgets;
	};
}