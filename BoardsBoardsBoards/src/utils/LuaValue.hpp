#pragma once

#include <variant>
#include <string>
#include <sstream>

#include "utils/Error.hpp"

namespace utils
{
	// Encapsulates an std::variant to make a bridge with the underlying Lua stack being used
	class LuaValue
	{
	public:
		using Variant = std::variant<int, float, bool, std::string>;
		//TODO: use std::format to instantiate the label
		LuaValue(const Variant& var) : entry(var), label(LabelFromVariant(var)) {}
		LuaValue(int i) : entry(i), label(LabelFromVariant(entry)) {}
		LuaValue(std::size_t s) : LuaValue(int(s)) {}
		LuaValue(bool b) : entry(b), label(LabelFromVariant(entry)) {}
		LuaValue(float fl) : entry(fl), label(LabelFromVariant(entry)) {}
		LuaValue(const std::string& str, std::size_t forced_index = 3) : entry(VariantFromString(str, forced_index)), label(LabelFromVariant(entry)) {}
		LuaValue(const char* str, std::size_t forced_index = 3) : LuaValue(std::string(str), forced_index) {}
		LuaValue() : entry(0), label("") {}

		//TODO exceptions that trace expected type of value vs actual type of value
		std::size_t index() const { return entry.index(); }
		bool isInt() const { return entry.index() == 0; }
		bool isFloat() const { return entry.index() == 1; }
		bool isBool() const { return entry.index() == 2; }
		bool isString() const { return entry.index() == 3; }

		int asInt() const { return std::get<int>(entry); }
		float asFloat() const { return std::get<float>(entry); }
		bool asBool() const { return std::get<bool>(entry); }
		const std::string& asString() const { return std::get<std::string>(entry); }
		const std::string& GetLabel() const { return label; }
		std::string GetPrettyType() const
		{
			switch (index())
			{
			case 0:
				return "Integer";
			case 1:
				return "Float";
			case 2:
				return "Boolean";
			case 3:
				return "String";
			default:
				return "Unknown Tag Type";
			}
		}

		bool operator== (const LuaValue& rhs) const
		{
			return ((index() == rhs.index()) && (entry == rhs.entry));
		}
		bool operator== (int rhs) const
		{
			if (isFloat()) return asFloat() == rhs;
			return (isInt() && asInt() == rhs);
		}
		bool operator== (std::size_t rhs) const
		{
			return operator==((int)rhs);
		}
		bool operator== (float rhs) const
		{
			if (isInt()) return asInt() == rhs;
			return (isFloat() && asFloat() == rhs);
		}
		bool operator== (bool rhs) const
		{
			return (isBool() && asBool() == rhs);
		}
		bool operator== (const char* rhs) const
		{
			return (isString() && asString() == rhs);
		}
		bool operator== (const std::string& rhs) const
		{
			return (isString() && asString() == rhs);
		}

	private:
		Variant entry;
		std::string label;

		static std::string LabelFromVariant(const std::variant<int, float, bool, std::string>& var)
		{
			switch (var.index())
			{
			case 0:
				return std::to_string(std::get<int>(var));
			case 1:
				return std::to_string(std::get<float>(var));
			case 2:
				if (std::get<bool>(var)) return "true";
				return "false";
			case 3:
				return std::get<std::string>(var);
			default:
				return "";
			}
		}
		static Variant VariantFromString(const std::string& input, std::size_t forced_index)
		{
			if (forced_index > 3 || forced_index < 0)
			{
				std::stringstream error;
				error << "A Tag cannot be initialized with index " << forced_index << ".";
				throw utils::TagError(error.str());
			}
			Variant ret;
			if (input == "false")
			{
				ret = false;
			}
			else if (input == "true")
			{
				ret = true;
			}
			else
			{
				if (forced_index == 2)
				{
					throw utils::TagError("Could not parse string into boolean Tag.");
				}
				if (forced_index == 3)
				{
					ret = input;
				}
				else
				{
					float fl = std::stof(input);
					float fl_trunc = std::trunc(fl);
					if (fl != fl_trunc || forced_index == 1)
					{
						ret = fl;
					}
					else
					{
						ret = (int)fl_trunc;
					}
				}
			}
			return ret;
		}
	};
}