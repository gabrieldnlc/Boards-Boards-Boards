#pragma once

#include <unordered_map>

#include "utils/LuaValue.hpp"
#include "utils/Error.hpp"
#include "containers/LuaVector.hpp"


namespace sb
{
	using utils::LuaValue;

	class TagEntryList : public LuaVector<LuaValue>
	{
	public:
		TagEntryList() : LuaVector<LuaValue>(false) {}
	};

	class Tags
	{
	public:
		using iterator = std::unordered_map<std::string, TagEntryList>::iterator;
		using const_iterator = std::unordered_map<std::string, TagEntryList>::const_iterator;
		
		bool HasTag(const std::string& key) const
		{
			return tags.contains(key);
		}
		
		iterator begin() { return tags.begin(); }
		iterator end() { return tags.end(); }

		const_iterator begin() const { return tags.cbegin(); }
		const_iterator end() const { return tags.cend(); }
		bool Empty() const { return tags.empty(); }
		bool RemoveKey(const std::string& key)
		{
			return tags.erase(key) == 1;
		}

		bool RemoveIndexFromKey(const std::string& key, std::size_t index)
		{
			if (!tags.contains(key)) return false;
			auto& values = tags[key];
			if (index < 0 || index >= values.size())
			{
				std::stringstream ss;
				ss << "The Tag Entry List has no index " << index << ".";
				throw std::out_of_range(ss.str());
			}
			auto pos = values.begin();
			if (index > 0) pos = std::next(pos, index);
			values.Erase(pos);
			if (values.Empty()) tags.erase(key);
			return true;
			
		}
		bool RemoveFromKey(const std::string& key, const LuaValue& value)
		{
			bool deleted = false;
			if (!tags.contains(key)) return deleted;
			auto& entries = tags[key];
			auto it = std::begin(entries);
			while (it != std::end(entries))
			{
				const LuaValue& entry = *it;
				if (entry == value)
				{
					entries.Erase(it);
					deleted = true;
					if (entries.Empty())
					{
						tags.erase(key);
					}
					break;
				}
				it = std::next(it);
			}
			return deleted;
		}
		TagEntryList& operator[](const std::string& key)
		{
			return tags[key];
		}
		const TagEntryList& operator[](const std::string& key) const
		{
			try
			{
				return tags.at(key);
			}
			catch (std::out_of_range&)
			{
				throw std::out_of_range("'" + key + "' is not a valid key nor can it be created on const object");
			}

		}

		bool HasConnection() const
		{
			return (HasTag("connects_to") && !tags.at("connects_to").Empty()); //TODO use strategy design pattern to change what a "connection" means
		}

		bool operator== (const Tags& t2) const
		{
			return tags == t2.tags;
		}

		bool Erase(const std::string& key)
		{
			return (tags.erase(key)) == 1;
		}
	private:
		std::unordered_map<std::string, TagEntryList> tags;
	};
}