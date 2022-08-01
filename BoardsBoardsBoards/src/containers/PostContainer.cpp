#include <sstream>

#include "PostContainer.hpp"
#include "utils/Error.hpp"


namespace board
{
	using utils::PostContainerError;
		
	PostContainer::iterator PostContainer::IteratorFromIndex(std::size_t idx)
	{
		try
		{
			return posts.IteratorFromIndex(idx);
		}
		catch (const LuaVectorError& err)
		{
			std::stringstream error;
			error << "PostContainer: " << err.what();
			throw PostContainerError(error.str());
		}
		
	}

	PostContainer::const_iterator PostContainer::IteratorFromIndex(std::size_t idx) const
	{
		try
		{
			return posts.IteratorFromIndex(idx);
		}
		catch (const LuaVectorError& err)
		{
			std::stringstream error;
			error << "PostContainer: " << err.what();
			throw PostContainerError(error.str());
		}
	}
	
	PostContainer::iterator PostContainer::MoveToLastPosition(iterator pos)
	{
		if (pos == end())
		{
			throw PostContainerError("PostContainer: cannot operate on end iterator\n;");
		}

		Post copy = *pos;
		std::size_t old_idx = copy.GetIdx();

		auto new_pos = Insert(end(), std::move(copy));

		std::size_t new_idx = new_pos->GetIdx();

		std::unordered_map<std::size_t, std::size_t> change;
		change[old_idx] = new_idx;

		ReconnectNodes(change);

		Erase(IteratorFromIndex(old_idx));

		return (posts.end() - 1);

	}

	PostContainer::iterator PostContainer::MoveToLastPosition(std::size_t pos)
	{
		return MoveToLastPosition(IteratorFromIndex(pos));
	}


	void PostContainer::PropagateIndexShift(iterator start, int offset, std::size_t removed_node_idx)
	{
		PropagateIndexShift(start, end(), offset, removed_node_idx);
	}

	void PostContainer::PropagateIndexShift(iterator start, iterator end, int offset, std::size_t removed_node_idx)
	{
		std::unordered_map<std::size_t, std::size_t> changes;
		if (removed_node_idx != 0)
		{
			changes[removed_node_idx] = 0;
		}
		while (start != end && start != this->end())
		{
			Post& post = *(start);
			int old_idx = post.GetIdx();
			post.IndexShift(offset);
			int new_idx = post.GetIdx();
			changes[old_idx] = new_idx;

			start = std::next(start);
		}
		ReconnectNodes(changes);
	}

	void PostContainer::ReconnectNodes(std::unordered_map<std::size_t, std::size_t>& index_changes)
	{
		std::vector<LuaVector<PostConnection>::iterator> to_delete;

		for (auto it = connections.begin(); it != connections.end(); it++)
		{
			auto& connection = *it;
			auto& from = connection.from;
			auto& to = connection.to;

			auto CheckIdx = [&index_changes, &to_delete, &it](std::size_t& idx) -> void
			{
				if (index_changes.contains(idx))
				{
					auto new_idx = index_changes[idx];
					if (new_idx == 0)
					{
						to_delete.emplace_back(it);
						return;
					}
					idx = new_idx;

				}
			};

			CheckIdx(from);
			CheckIdx(to);
		}

		for (int i = to_delete.size() - 1; i >= 0; i--)
		{
			connections.Erase(to_delete[i]);
		}
	}

	bool PostContainer::operator== (const PostContainer& rhs) const
	{
		if (size() != rhs.size()) return false;
		if (board_options != rhs.board_options) return false;
		if (connections != rhs.connections) return false;
		for (std::size_t i = 1; i <= size(); i++)
		{
			const Post& p1 = posts[i];
			const Post& p2 = rhs[i];
			if (p1 != p2) return false;
		}
		return true;
	}

	PostContainer::iterator PostContainer::Insert(iterator pos, const Post& post)
	{
		Post to_move = post;
		return Insert(pos, std::move(to_move));
	}

	PostContainer::iterator PostContainer::Insert(iterator pos, Post&& post)
	{
		PostContainer::iterator it = posts.Insert(pos, std::move(post));
		Post& new_post = *(it);
		if (it == begin())
		{
			new_post.SetIdx(1);
		}
		else if (std::next(it) == end())
		{
			new_post.SetIdx(size());
		}
		else
		{
			new_post.SetIdx(it - begin() + 1);
		}

		PropagateIndexShift(std::next(it), +1);

		return it;

	}

	PostContainer::iterator PostContainer::CreatePostBack(std::string&& content)
	{
		return CreatePost(end(), std::move(content));
	}

	PostContainer::iterator PostContainer::CreatePostBack(const std::string& content)
	{
		return CreatePost(end(), content);
	}

	PostContainer::iterator PostContainer::CreatePost(iterator pos, std::string&& content)
	{
		Post to_move(std::move(content));
		return Insert(pos, std::move(to_move));
	}

	PostContainer::iterator PostContainer::CreatePost(iterator pos, const std::string& content)
	{
		Post to_move(content);
		return Insert(pos, std::move(to_move));
	}

	PostContainer::iterator PostContainer::Erase(iterator pos)
	{
		if (pos == end())
		{
			throw PostContainerError("Cannot operate on end iterator");
		}
		std::size_t removed_node_idx = pos->GetIdx();
		iterator ret = posts.Erase(pos);

		PropagateIndexShift(ret, -1, removed_node_idx);

		return ret;
	}

	void PostContainer::PopBack()
	{
		if (Empty()) return;
		Erase(end() - 1);
	}
	void PostContainer::Resize(int count)
	{
		posts.Resize(count);
	}

}