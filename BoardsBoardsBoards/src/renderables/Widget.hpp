#pragma once

#include <string>

namespace board
{
	class Widget
	{
	public:
		Widget(std::string ID) : ID(std::move(ID)) {}
		virtual void Render() = 0;
		std::string ID;
		bool is_open = true;
	};
}