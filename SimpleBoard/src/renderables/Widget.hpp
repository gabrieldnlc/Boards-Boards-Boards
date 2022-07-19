#pragma once

#include <string>

namespace sb
{
	class Widget
	{
	public:
		Widget(const std::string& ID) : ID(ID) {}
		Widget(std::string&& ID) : ID(std::move(ID)) {}
		virtual void Render() = 0;
		std::string ID;
		bool is_open = true;
	};
}