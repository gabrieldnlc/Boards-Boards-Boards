#pragma once

#include <algorithm> // std::equal

#include "imgui.h"

namespace utils
{
	class BoardColors // Based around ImColor from imgui.h.
	{
	public:

		BoardColors()
		{
			for (int i = 0; i < IM_ARRAYSIZE(palette); i++)
			{
				palette[i].w = 1.0f;
				ColorConvertHSVtoRGB((i / 31.0f), 0.8f, 0.8f, palette[i].x, palette[i].y, palette[i].z);
			}
		}

		// Originally ImGui::ColorConvertHSVtoRGB
		static void ColorConvertHSVtoRGB(float h, float s, float v, float& out_r, float& out_g, float& out_b)
		{
			if (s == 0.0f)
			{
				out_r = out_g = out_b = v;
				return;
			}

			h = fmodf(h, 1.0f) / (60.0f / 360.0f);
			int   i = (int)h;
			float f = h - (float)i;
			float p = v * (1.0f - s);
			float q = v * (1.0f - s * f);
			float t = v * (1.0f - s * (1.0f - f));

			switch (i)
			{
			case 0: out_r = v; out_g = t; out_b = p; break;
			case 1: out_r = q; out_g = v; out_b = p; break;
			case 2: out_r = p; out_g = v; out_b = t; break;
			case 3: out_r = p; out_g = q; out_b = v; break;
			case 4: out_r = t; out_g = p; out_b = v; break;
			case 5: default: out_r = v; out_g = p; out_b = q; break;
			}
		}


		static float RGBIntToFloat(int v) 
		{
			return (float)v * (1.0f / 255.0f);
		}
		static int RGBFloatToInt(float v)
		{
			return (int)(v * (255.0f / 1.0f));
		}

		static inline ImColor ArrayToImColor(float arr[3])
		{
			return ImColor(arr[0], arr[1], arr[2]);
		}

		bool operator==(const BoardColors& rhs) const
		{
			const bool post_equals = std::equal(std::begin(post), std::end(post), std::begin(rhs.post));
			const bool bg_equals = std::equal(std::begin(bg), std::end(bg), std::begin(rhs.bg));
			const bool connection_equals = std::equal(std::begin(connection), std::end(connection), std::begin(rhs.connection));
			const bool selected_connection_equals = std::equal(std::begin(selected_connection), std::end(selected_connection), std::begin(rhs.selected_connection));
			const bool text_equals = std::equal(std::begin(text), std::end(text), std::begin(rhs.text));
			return (post_equals && bg_equals && connection_equals && selected_connection_equals && text_equals);
		}

		float text[3]{ RGBIntToFloat(255), RGBIntToFloat(255), RGBIntToFloat(255) };
		float post[3] { RGBIntToFloat(35), RGBIntToFloat(53), RGBIntToFloat(114)};
		float bg[3]{ RGBIntToFloat(30), RGBIntToFloat(30), RGBIntToFloat(30) };
		float connection[3]{ RGBIntToFloat(255), RGBIntToFloat(255), RGBIntToFloat(255) };
		float selected_connection[3]{ RGBIntToFloat(15), RGBIntToFloat(15), RGBIntToFloat(15) };

		ImVec4 palette[32];
	};
}