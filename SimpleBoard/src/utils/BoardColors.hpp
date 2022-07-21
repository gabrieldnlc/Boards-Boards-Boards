#pragma once

#include <algorithm> // std::equal

#include "imgui.h"

namespace utils
{
	class BoardColors // Based around ImColor from imgui.h.
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

		bool operator==(const BoardColors& another_table) const
		{
			const bool post_equals = std::equal(std::begin(post), std::end(post), std::begin(another_table.post));
			const bool bg_equals = std::equal(std::begin(bg), std::end(bg), std::begin(another_table.bg));
			const bool connection_equals = std::equal(std::begin(connection), std::end(connection), std::begin(another_table.connection));
			const bool selected_connection_equals = std::equal(std::begin(selected_connection), std::end(selected_connection), std::begin(another_table.selected_connection));

			return (post_equals && bg_equals && connection_equals && selected_connection_equals);
		}

		float post[3] { RGBIntToFloat(35), RGBIntToFloat(53), RGBIntToFloat(114)};
		float bg[3]{ RGBIntToFloat(30), RGBIntToFloat(30), RGBIntToFloat(30) };
		float connection[3]{ RGBIntToFloat(255), RGBIntToFloat(255), RGBIntToFloat(255) };
		float selected_connection[3]{ RGBIntToFloat(15), RGBIntToFloat(15), RGBIntToFloat(15) };

	};
}