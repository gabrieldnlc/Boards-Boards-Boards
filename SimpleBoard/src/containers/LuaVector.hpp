#pragma once

#include <vector>

#include "utils/Error.hpp"

namespace sb {

	using utils::LuaVectorError;

	template<typename T>
	class LuaVector
		//TODO due to the specialization of vector<bool> on the Standard Library, LuaVector<bool> is currently not possible.
	{
	public:
		LuaVector(T&& element, bool auto_resize = true) : items{ std::move(element) }, auto_resize(auto_resize) {}
		LuaVector(const T& element, bool auto_resize = true) : items{ element }, auto_resize(auto_resize) {}

		LuaVector(const std::vector<T>& elements, bool auto_resize = true) : items(elements), auto_resize(auto_resize) {}

		LuaVector(bool auto_resize = true) : auto_resize(auto_resize) {}

		bool operator==(const LuaVector<T>& other) const
		{
			return items == other.items;
		}
		using iterator = std::vector<T>::iterator;

		iterator IteratorFromIndex(std::size_t pos)
		{
			if (pos == size() + 1) return end();
			if (Empty() || pos == 0 || pos > size() + 1) throw LuaVectorError("LuaVector does not have index " + std::to_string(pos) + ".");
			auto it = begin();
			if (pos == 1) return it;
			return std::next(it, pos - 1);
		}

		iterator begin() { return items.begin(); }
		iterator end() { return items.end(); }
		T& operator[](int idx) { return at(idx); }
		T& front() { return items.front(); }
		T& back() { return items.back(); }
		void SetAutoResize(bool resize) { auto_resize = resize; }
		T& at(int count)
		{
			try
			{
				return items.at(offset_idx(count));
			}
			catch (const std::out_of_range&)
			{
				if (auto_resize && count > 0)
				{
					items.resize(count);
					return items.at(offset_idx(count));
				}
				else
				{
					throw LuaVectorError("LuaVector does not have index " + std::to_string(count) + ".");
				}
			}
		}
		using const_iterator = std::vector<T>::const_iterator;
		const_iterator begin() const { return items.cbegin(); } 
		const_iterator end() const { return items.cend(); }		

		const_iterator IteratorFromIndex(std::size_t pos) const
		{
			if (pos == size() + 1) return end();
			if (Empty() || pos == 0 || pos > size() + 1) throw LuaVectorError("LuaVector does not have index " + std::to_string(pos) + ".");
			auto it = begin();
			if (pos == 1) return it;
			return std::next(it, pos - 1);
		}

		const T& operator[](int idx) const { return at(idx); }		
		const T& front() const { return items.front(); }		
		const T& back() const { return items.back(); }
		const T& at(int count) const
		{
			try
			{
				return items.at(offset_idx(count));
			}
			catch (const std::out_of_range&)
			{
				throw LuaVectorError("LuaVector does not have index " + std::to_string(count) + ".");
			}
		}

		std::size_t size() const { return items.size(); }
		bool Empty() const { return items.empty(); }
		void Clear() { items.clear(); }
		iterator Insert(iterator pos, T&& value)
		{
			return items.insert(pos, std::move(value));
		}
		iterator Insert(iterator pos, const T& value)
		{
			return items.insert(pos, value);
		}
		template<class... Args>
		iterator Emplace(iterator pos, Args&&... args)
		{
			return items.emplace(pos, std::forward<Args>(args)...);
		}
		iterator Erase(iterator pos)
		{
			return items.erase(pos);
		}
		void PushBack(T&& value)
		{
			items.push_back(std::move(value));
		}
		void PushBack(const T& value)
		{
			items.push_back(value);
		}
		template<class... Args>
		iterator EmplaceBack(Args&&... args)
		{
			return Emplace(std::end(*this), std::forward<Args>(args)...);
		}
		void PopBack()
		{
			items.pop_back();
		}
		void Resize(int count)
		{
			items.resize(count);
		}
		void ChangeAllValues(const T& value)
		{
			items.assign(items.size(), value);
		}
	private:
		std::vector<T> items;
		bool auto_resize;
		inline int offset_idx(int idx) const { return idx - 1; }
	};
}