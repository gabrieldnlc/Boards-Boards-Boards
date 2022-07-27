#include "ParsingStrategies.hpp"


namespace utils
{
	const PostContentIntoLuaTable ParsingStrategies::PostContentToTable = [](const LuaVector<PostContent>& post_content) -> sol::table
	{
		sol::table result = LuaStack::EmptyTable();
		for (std::size_t i = 1; i <= post_content.size(); i++)
		{
			const PostContent& content = post_content[i];
			switch (content.GetType())
			{
			case(ContentType::text):
			{
				result[i] = content.AsString();
			}
			break;
			default:
				throw SerializingError("Cannot serialize unknown ContentType");
			}
		}
		return result;
	};

	const LuaObjectIntoPost ParsingStrategies::ObjectToPost = [](sol::table object) -> Post
	{

		sol::object obj_content = object["content"];
		std::vector<std::string> content;

		switch (obj_content.get_type())
		{
		case(sol::type::string):
			content.push_back(obj_content.as<std::string>());
			break;
		case(sol::type::table):
		{
			auto table_content = obj_content.as<sol::table>();
			std::size_t size = table_content.size();

			for (std::size_t i = 1; i <= size; i++)
			{
				try
				{
					sol::object o = table_content[i];
					std::string c = o.as<std::string>();
					content.push_back(std::move(c));
				}
				catch (const std::exception&)
				{
					throw ParsingError("Post has no valid 'content' field.");
				}

			}

			break;
		}
		default:
			throw ParsingError("Post has no valid 'content' field.");
		}
		Post new_post = Post(content);
		sol::object tags = object["tags"];
		if (tags.valid())
		{
			sol::table tags_table = tags.as<sol::table>();
			for (const auto& pair : tags_table) {
				sol::object first = pair.first;
				std::string key = first.as<std::string>();
				try
				{
					sol::object second = pair.second;
					auto vars = VariantsFromLuaObject(second);
					auto& tags = new_post.tags[key];
					for (auto& var : vars)
					{
						tags.EmplaceBack(var);
					}
				}
				catch (const ParsingError& e)
				{
					throw ParsingError((": Cannot parse '" + key + "' key: " + e.what()));
				}

			}
		}

		sol::object color = object["color"];

		if (color.valid())
		{
			sol::table color_values = color.as<sol::table>();
			auto& color_array = new_post.color;
			if (color_values.size() != 3)
			{
				throw ParsingError("Post color is not valid.");
			}
			int r = color_values[1];
			int g = color_values[2];
			int b = color_values[3];

			color_array[0] = BoardColors::RGBIntToFloat(r);
			color_array[1] = BoardColors::RGBIntToFloat(g);
			color_array[2] = BoardColors::RGBIntToFloat(b);
		}
		else
		{
			new_post.color[0] = -1;
			new_post.color[1] = -1;
			new_post.color[2] = -1;
		}

		sol::object display_pos = object["display_pos"];
		if (!display_pos.valid())
		{
			new_post.display_pos.first = 0;
			new_post.display_pos.second = 0;
		}
		else
		{
			sol::table pos = display_pos.as<sol::table>();
			if (pos.size() != 2)
			{
				new_post.display_pos.first = 0;
				new_post.display_pos.second = 0;
			}
			else
			{
				new_post.display_pos.first = pos[1];
				new_post.display_pos.second = pos[2];
			}
		}
		return new_post;
	};

	const LuaTableIntoContainer ParsingStrategies::TableToContainer = [](sol::table table) -> board::PostContainer
	{
		using board::PostContainer;

		sol::object posts = table["posts"];
		if (!posts.valid())
		{
			throw ParsingError("The file does not have a Posts table.");
		}
		sol::table posts_table = posts;
		std::size_t raw_size = posts_table.size();
		PostContainer container;
		container.Resize(raw_size);

		for (std::size_t i = 1; i <= raw_size; i++)
		{
			sol::object obj = posts_table[i];
			Post new_post = ParsingStrategies::ObjectToPost(obj);
			new_post.SetIdx(i);

			container[i] = std::move(new_post);
		}

		sol::object config = table["board_config"];
		if (config.valid())
		{
			auto& color_table = container.board_options.color_table;
			sol::table board_config = config;
			sol::object bg = board_config["bg_color"];

			if (bg.valid())
			{
				auto& color = color_table.bg;
				sol::table bg_color = bg;
				color[0] = BoardColors::RGBIntToFloat(bg_color[1]);
				color[1] = BoardColors::RGBIntToFloat(bg_color[2]);
				color[2] = BoardColors::RGBIntToFloat(bg_color[3]);
			}

			sol::object post = board_config["post_color"];

			if (post.valid())
			{
				auto& color = color_table.post;
				sol::table post_color = post;
				color[0] = BoardColors::RGBIntToFloat(post_color[1]);
				color[1] = BoardColors::RGBIntToFloat(post_color[2]);
				color[2] = BoardColors::RGBIntToFloat(post_color[3]);
 			}

			sol::object connection = board_config["connection_color"];

			if (connection.valid())
			{
				auto& color = color_table.connection;
				sol::table connection_color = connection;
				color[0] = BoardColors::RGBIntToFloat(connection_color[1]);
				color[1] = BoardColors::RGBIntToFloat(connection_color[2]);
				color[2] = BoardColors::RGBIntToFloat(connection_color[3]);
			}

			sol::object selected_connection = board_config["selected_connection_color"];

			if (selected_connection.valid())
			{
				auto& color = color_table.selected_connection;
				sol::table sel_connection_color = selected_connection;
				color[0] = BoardColors::RGBIntToFloat(sel_connection_color[1]);
				color[1] = BoardColors::RGBIntToFloat(sel_connection_color[2]);
				color[2] = BoardColors::RGBIntToFloat(sel_connection_color[3]);
			}

			sol::object text = board_config["text_color"];

			if (text.valid())
			{
				auto& color = color_table.text;
				sol::table text_color = text;
				color[0] = BoardColors::RGBIntToFloat(text_color[1]);
				color[1] = BoardColors::RGBIntToFloat(text_color[2]);
				color[2] = BoardColors::RGBIntToFloat(text_color[3]);
			}

		}

		sol::object connections = table["connections"];
		if (connections.valid())
		{
			sol::table connections_table = connections;
			for (auto connection : connections_table)
			{
				sol::table pair = connection.second.as<sol::table>();
				std::size_t from = pair[1];
				std::size_t to = pair[2];
				container.connections.EmplaceBack(from, to);
			}
		}

		return container;
	};

	const ContainerIntoLuaTable ParsingStrategies::ContainerToTable = [](const PostContainer& container) -> sol::table
	{
		
		sol::table posts_table = LuaStack::EmptyTable();
		for (std::size_t i = 1; i <= container.size(); i++)
		{
			const Post& post = container[i];

			posts_table[i] = LuaStack::EmptyTable();
			sol::table curr = posts_table[i];

			curr["content"] = PostContentToTable(post.content);

			if (!post.tags.Empty())
			{
				curr["tags"] = LuaStack::EmptyTable();
				sol::table t = curr["tags"];
				for (const auto& it : post.tags)
				{
					const auto& key = it.first;
					const auto& tags = it.second;
					sol::table key_entries = LuaStack::EmptyTable();
					for (const auto& entry : tags)
					{
						switch (entry.index())
						{
						case 0:
							key_entries.add(entry.asInt());
							break;
						case 1:
							key_entries.add(entry.asFloat());
							break;
						case 2:
							key_entries.add(entry.asBool());
							break;
						case 3:
							key_entries.add(entry.asString());
							break;
						}
					}
					t[key] = key_entries;

				}
			}

			curr["display_pos"] = LuaStack::EmptyTable();
			curr["display_pos"][1] = post.display_pos.first;
			curr["display_pos"][2] = post.display_pos.second;

			const bool has_color = post.color[0] >= 0 && post.color[1] >= 0 && post.color[2] >= 0;

			if (has_color)
			{
				curr["color"] = LuaStack::EmptyTable();
				curr["color"][1] = BoardColors::RGBFloatToInt(post.color[0]);
				curr["color"][2] = BoardColors::RGBFloatToInt(post.color[1]);
				curr["color"][3] = BoardColors::RGBFloatToInt(post.color[2]);
			}
			
		}

		sol::table board_config = LuaStack::EmptyTable();

		auto& color_table = container.board_options.color_table;
		board_config["bg_color"] = LuaStack::EmptyTable();
		board_config["post_color"] = LuaStack::EmptyTable();
		board_config["connection_color"] = LuaStack::EmptyTable();
		board_config["selected_connection_color"] = LuaStack::EmptyTable();
		board_config["text_color"] = LuaStack::EmptyTable();
		
		auto& bg_color = color_table.bg;
		board_config["bg_color"][1] = color_table.RGBFloatToInt(bg_color[0]);
		board_config["bg_color"][2] = color_table.RGBFloatToInt(bg_color[1]);
		board_config["bg_color"][3] = color_table.RGBFloatToInt(bg_color[2]);

		auto& post_color = color_table.post;
		board_config["post_color"][1] = color_table.RGBFloatToInt(post_color[0]);
		board_config["post_color"][2] = color_table.RGBFloatToInt(post_color[1]);
		board_config["post_color"][3] = color_table.RGBFloatToInt(post_color[2]);

		auto& connection_color = color_table.connection;
		board_config["connection_color"][1] = color_table.RGBFloatToInt(connection_color[0]);
		board_config["connection_color"][2] = color_table.RGBFloatToInt(connection_color[1]);
		board_config["connection_color"][3] = color_table.RGBFloatToInt(connection_color[2]);

		auto& sel_connection_color = color_table.selected_connection;
		board_config["selected_connection_color"][1] = color_table.RGBFloatToInt(sel_connection_color[0]);
		board_config["selected_connection_color"][2] = color_table.RGBFloatToInt(sel_connection_color[1]);
		board_config["selected_connection_color"][3] = color_table.RGBFloatToInt(sel_connection_color[2]);

		auto& text_color = color_table.text;
		board_config["text_color"][1] = color_table.RGBFloatToInt(text_color[0]);
		board_config["text_color"][2] = color_table.RGBFloatToInt(text_color[1]);
		board_config["text_color"][3] = color_table.RGBFloatToInt(text_color[2]);

		sol::table connections = LuaStack::EmptyTable();
		for (const PostContainer::PostConnection& connection : container.connections)
		{
			sol::table c = LuaStack::EmptyTable();
			c[1] = connection.from;
			c[2] = connection.to;
			connections.add(c);
		}

		sol::table file = LuaStack::EmptyTable();
		file["posts"] = posts_table;
		file["board_config"] = board_config;
		file["connections"] = connections;

		return file;
	};
}