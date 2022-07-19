#pragma once

#include <variant>
#include <string>
#include <utility> // std pair

#include "utils/Error.hpp"

namespace sb
{
	using std::string;
	using ImageInfo = std::pair<std::string, std::string>;

	enum class ContentType
	{
		text, image
	};

	class PostContent
	{
	public:

		PostContent(string str = "") : data(std::move(str)) {}

		PostContent(string image_path, string image_comment)
			: data(std::move(std::pair<string, string>(std::move(image_path), std::move(image_comment)))) {}

		ContentType GetType() const
		{
			switch (data.index())
			{
			case (0):
				return ContentType::text;
			case (1):
				return ContentType::image;
			default:
				throw utils::PostContentError("PostContent: unknown content type.");
			}
		}

		const char* GetPrettyType() const
		{
			switch (data.index())
			{
			case (0):
				return "Text";
			case (1):
				return "Image";
			default:
				throw utils::PostContentError("PostContent: unknown content type.");
			}
		}

		bool IsString() const
		{
			return data.index() == 0;
		}

		const std::string& AsString() const
		{
			return std::get<string>(data);
		}

		std::string& AsString()
		{
			return std::get<string>(data);
		}

		bool IsImageInfo() const
		{
			return data.index() == 1;
		}

		const ImageInfo& AsImageInfo() const
		{
			return std::get<ImageInfo>(data);
		}

		ImageInfo& AsImageInfo()
		{
			return std::get<ImageInfo>(data);
		}

		bool operator==(const PostContent& another) const
		{
			return data == another.data;
		}

		bool operator==(const std::string& str) const
		{
			if (!IsString()) return false;
			return AsString() == str;
		}

		bool operator==(const ImageInfo& info) const
		{
			if (!IsImageInfo()) return false;
			return AsImageInfo() == info;
		}

	private:
		std::variant<string, ImageInfo> data;
	};
}
