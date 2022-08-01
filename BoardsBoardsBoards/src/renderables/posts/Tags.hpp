#pragma once

#include <unordered_map>

#include "utils/LuaValue.hpp"
#include "utils/Error.hpp"
#include "containers/LuaVector.hpp"


namespace board
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
		iterator begin() { return tags.begin(); }
		iterator end() { return tags.end(); }


		using const_iterator = std::unordered_map<std::string, TagEntryList>::const_iterator;
		const_iterator begin() const { return tags.cbegin(); }
		const_iterator end() const { return tags.cend(); }


		bool HasTag(const std::string& key) const
		{
			return tags.contains(key);
		}
		bool Empty() const { return tags.empty(); }

		// Returns true if key existed and was removed
		bool RemoveKey(const std::string& key)
		{
			return tags.erase(key) == 1;
		}

		// Returns false if key does not exist
		// Throws if index > tags[key].size()
		// Will remove key from tags if after removal tags[key].size() == 0
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

		// Returns true if tags[key] contained value and value was successfully removed
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
			return (HasTag("connects_to") && !tags.at("connects_to").Empty());
		}

		bool operator== (const Tags& rhs) const
		{
			return tags == rhs.tags;
		}

		// Returns true if tags contained key and key was successfully deleted
		bool Erase(const std::string& key)
		{
			return (tags.erase(key)) == 1;
		}

	private:

		std::unordered_map<std::string, TagEntryList> tags;
	};
}