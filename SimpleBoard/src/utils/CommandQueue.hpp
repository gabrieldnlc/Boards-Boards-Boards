#pragma once

#include <utility>
#include <queue>
#include <string>

#include "utils/Error.hpp"

namespace sb
{
	class CommandQueue
	{ 
	public:
		enum class commands
		{
			openFile, closeAllTabs, createErrorWindow, openBoardOptions
		};
		enum class targets 
		{
			applicationLayer, widgetManager, currentTab
		};

		using CommandPair = std::pair<CommandQueue::commands, std::string>;
		using Queue = std::queue<CommandPair>;

		// alias function
		static void CreateErrorWindow(const std::string& error)
		{
			addToQueue(CommandQueue::targets::widgetManager, CommandQueue::commands::createErrorWindow, error);
		}
		static void addToQueue(enum targets target, enum commands operation, const std::string& data = "")
		{
			get(target).emplace(operation, data);
		}
		static CommandPair& front(enum targets target) {	return get(target).front();	}
		static void pop(enum targets target) { get(target).pop(); }
		static bool empty(enum targets target)	{ return get(target).empty(); }
	private:
		static inline Queue& get(enum targets target)
		{
			switch (target)
			{
			case (targets::applicationLayer):
				return appLevelCommands;
			case (targets::widgetManager):
				return windowsLevelCommands;
			case(targets::currentTab):
				return currentTabCommands;
			default:
				throw utils::CommandQueueError("CommandQueueError: Target of command is unknown.");
			}
		}
		static inline Queue appLevelCommands;
		static inline Queue windowsLevelCommands;
		static inline Queue currentTabCommands;
	};
}