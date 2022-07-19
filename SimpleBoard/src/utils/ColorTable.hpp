#pragma once

#include <algorithm> // std::equal

#include "imgui.h"

namespace utils
{
	class ColorTable // Based around ImColor from imgui.h.
	{
	public:

		static float RGBIntToFloat(int v) 
		{
			return (float)v * (1.0f / 255.0f);
		}
		static int RGBFloatToInt(float v)
		{
			return (int)(v * (255.0f / 1.0f));
		}

		static ImColor ArrayToImColor(float arr[3])
		{
			return ImColor(arr[0], arr[1], arr[2]);
		}

		bool operator==(const ColorTable& another_table) const
		{
			bool post_color_equals = std::equal(std::begin(post_color), std::end(post_color), std::begin(another_table.post_color));
			bool bg_color_equals = std::equal(std::begin(bg_color), std::end(bg_color), std::begin(another_table.bg_color));

			return (post_color_equals && bg_color_equals);
		}

		float post_color[3] { RGBIntToFloat(35), RGBIntToFloat(53), RGBIntToFloat(114)};
		float bg_color[3]{ RGBIntToFloat(30), RGBIntToFloat(30), RGBIntToFloat(30) };

	};
}