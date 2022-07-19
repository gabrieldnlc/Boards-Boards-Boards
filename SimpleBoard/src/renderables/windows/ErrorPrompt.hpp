#pragma once

#include "Prompt.hpp"

namespace sb
{
	class ErrorPrompt : public Prompt
	{
	public:
		ErrorPrompt(const std::string& what, const std::string& ID) : Prompt(ID), what(what) {}
		void Render() override;
		std::string what;
	};
}