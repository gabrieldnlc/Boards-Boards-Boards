#pragma once

#include <utility>
#include <queue>
#include <string>

#include "utils/Error.hpp"

namespace sb
{
	class OperationQueue
	{ //TODO transform this whole system into just callbacks and stuff
	public:
		enum class operations
		{
			openFile, closeAllTabs, createErrorWindow
		};
		enum class targets //TODO maybe a better name for this enum
		{
			applicationLayer, widgetManager
		};

		using operationPair = std::pair<OperationQueue::operations, std::string>;
		using queue = std::queue<operationPair>;

		// alias function
		static void CreateErrorWindow(const std::string& error)
		{
			addToQueue(OperationQueue::targets::widgetManager, OperationQueue::operations::createErrorWindow, error);
		}
		static void addToQueue(enum targets target, enum operations operation, const std::string& data = "")
		{
			get(target).emplace(operation, data);
		}
		static operationPair& front(enum targets target) {	return get(target).front();	}
		static void pop(enum targets target) { get(target).pop(); }
		static bool empty(enum targets target)	{ return get(target).empty(); }
	private:
		static inline queue& get(enum targets target)
		{
			switch (target)
			{
			case (targets::applicationLayer):
				return appLevelOperations;
			case (targets::widgetManager):
				return windowsLevelOperations;
			default:
				throw utils::OperationQueueError();
			}
		}
		static inline queue appLevelOperations;
		static inline queue windowsLevelOperations;
	};
}