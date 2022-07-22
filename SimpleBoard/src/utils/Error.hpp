#pragma once

#include <stdexcept>
#include <string>
#include <utility> // std move

namespace utils
{
	class Error : public std::exception
	{  
	public:
		const std::string msg;
		Error(std::string msg = "") : msg(std::move(msg)) {}
		const char* what() const noexcept override { return msg.data(); }
	};

	class ParsingError : public Error
	{
	public:
		ParsingError(std::string msg = "Could not parse target file.") : Error(std::move(msg)) {}
	};

	class LuaAccessError : public Error
	{
	public:
		LuaAccessError(std::string msg = "Invalid query into the Lua stack.") : Error(std::move(msg)) {}
	};
	
	class SerializingError : public Error
	{
	public:
		SerializingError(std::string msg = "Could not serialize.") : Error(std::move(msg)) {}
	};

	class DeserializingError : public Error
	{
	public:
		DeserializingError(std::string msg = "Could not deserialize.") : Error(std::move(msg)) {}
	};

	class FileOpenError : public Error
	{
	public:
		FileOpenError(std::string msg = "Could not open file.") : Error(std::move(msg)) {}
	};
	class FileWriteError : public Error
	{
	public:
		FileWriteError(std::string msg = "Could not write to file.") : Error(std::move(msg)) {}
	};
	class CommandQueueError : public Error
	{
	public:
		CommandQueueError(std::string msg = "Mismatched Target/Operation combination.") : Error(std::move(msg)) {}
	};

	class LuaVectorError : public Error
	{
	public:
		LuaVectorError(std::string msg = "LuaVector: unknown operation.") : Error(std::move(msg)) {}
	};

	class PostContainerError : public Error
	{
	public:
		PostContainerError(std::string msg = "Post Container: unknown operation.") : Error(std::move(msg)) {}
	};

	class BoardRenderError : public Error
	{
	public:
		BoardRenderError(std::string msg = "Could not render target board.") : Error(std::move(msg)) {}
	};

	class TagError : public Error
	{
	public:
		TagError(std::string msg = "Invalid operation on Tag instance.") : Error(std::move(msg)) {}
	};

	class PostContentError : public Error
	{
	public:
		PostContentError(std::string msg = "Could not operate on PostContent instance") : Error(std::move(msg)) {}
	};

}