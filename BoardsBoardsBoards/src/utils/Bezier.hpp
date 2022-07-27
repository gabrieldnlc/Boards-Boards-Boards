#pragma once

#include "imgui.h"
#include "imgui_internal.h" 

/*
	Code adapted from:
	https://github.com/Nelarius/imnodes/blob/master/imnodes.cpp
*/

namespace utils
{
	struct CubicBezier
	{
		ImVec2 P0, P1, P2, P3;
		int    NumSegments;
	};

	inline CubicBezier GetCubicBezier(ImVec2 start, ImVec2 end, const float line_segments_per_length = 5)
	{
		const float link_length = ImSqrt(ImLengthSqr(end - start));
		const ImVec2 offset = ImVec2(0.25f * link_length, 0.f);
		CubicBezier cubic_bezier;
		cubic_bezier.P0 = start;
		cubic_bezier.P1 = start + offset;
		cubic_bezier.P2 = end - offset;
		cubic_bezier.P3 = end;
		cubic_bezier.NumSegments = ImMax(static_cast<int>(link_length * line_segments_per_length), 1);
		return cubic_bezier;
	}

	inline ImVec2 EvalCubicBezier(
		const float   t,
		const ImVec2& P0,
		const ImVec2& P1,
		const ImVec2& P2,
		const ImVec2& P3)
	{
		// B(t) = (1-t)**3 p0 + 3(1 - t)**2 t P1 + 3(1-t)t**2 P2 + t**3 P3

		const float u = 1.0f - t;
		const float b0 = u * u * u;
		const float b1 = 3 * u * u * t;
		const float b2 = 3 * u * t * t;
		const float b3 = t * t * t;
		return ImVec2(
			b0 * P0.x + b1 * P1.x + b2 * P2.x + b3 * P3.x,
			b0 * P0.y + b1 * P1.y + b2 * P2.y + b3 * P3.y);
	}

	ImVec2 GetClosestPointOnCubicBezier(const int num_segments, const ImVec2& p, const CubicBezier& cb)
	{
		ImVec2 p_last = cb.P0;
		ImVec2 p_closest;
		float  p_closest_dist = FLT_MAX;
		float  t_step = 1.0f / (float)num_segments;
		for (int i = 1; i <= num_segments; ++i)
		{
			ImVec2 p_current = EvalCubicBezier(t_step * i, cb.P0, cb.P1, cb.P2, cb.P3);
			ImVec2 p_line = ImLineClosestPoint(p_last, p_current, p);
			float  dist = ImLengthSqr(p - p_line);
			if (dist < p_closest_dist)
			{
				p_closest = p_line;
				p_closest_dist = dist;
			}
			p_last = p_current;
		}
		return p_closest;
	}

	inline ImRect GetContainingRectForCubicBezier(const CubicBezier& b, float minimum_distance)
	{
		const ImVec2 min = ImVec2(ImMin(b.P0.x, b.P3.x), ImMin(b.P0.y, b.P3.y));
		const ImVec2 max = ImVec2(ImMax(b.P0.x, b.P3.x), ImMax(b.P0.y, b.P3.y));

		ImRect rect(min, max);
		rect.Add(b.P1);
		rect.Add(b.P2);
		rect.Expand(minimum_distance);

		return rect;
	}

	inline float GetDistanceToCubicBezier(
		const ImVec2& pos,
		const CubicBezier& cubic_bezier,
		const int          num_segments)
	{
		const ImVec2 point_on_curve = GetClosestPointOnCubicBezier(num_segments, pos, cubic_bezier);

		const ImVec2 to_curve = point_on_curve - pos;
		return ImSqrt(ImLengthSqr(to_curve));
	}

	inline void DrawCubicBezier(const CubicBezier& bezier, ImU32 col = IM_COL32_WHITE, float thickness = 5.f, ImDrawList* draw_list = ImGui::GetWindowDrawList())
	{
		draw_list->AddBezierCubic(bezier.P0, bezier.P1, bezier.P2, bezier.P3,
								  col, thickness, bezier.NumSegments);
	}

	bool IsMouseOnCubicBezier(const CubicBezier& b, ImVec2& mouse_pos, float minimum_distance)
	{
		const auto connection_rect = GetContainingRectForCubicBezier(b, minimum_distance);
		if (!connection_rect.Contains(mouse_pos)) return false;

		const float distance = GetDistanceToCubicBezier(mouse_pos, b, b.NumSegments);

		return (distance < minimum_distance);
	}

	
	
}

