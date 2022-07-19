#pragma once

#include <string>

#include "renderables/Widget.hpp"

namespace sb
{
	class Prompt : public Widget
	{
	public:
		Prompt(const std::string& ID) : Widget(ID) {}
		virtual void Render() = 0;
		
	protected:
		bool first_config = true;
	};
}