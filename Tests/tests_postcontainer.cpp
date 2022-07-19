#include <string>
#include <memory>
#include <utility>

#include "catch.hpp"

#include "utils/LuaStack.hpp"
#include "utils/parsing/BoardParser.hpp"
#include "utils/Error.hpp"
#include "containers/PostContainer.hpp"

using std::string;

using sb::Post;
using sb::PostContainer;

using utils::BoardParser;
using utils::LuaStack;

const string tag = "[PostContainer]";

auto AreIndexesValid = [](PostContainer& c) -> bool
{
	for (std::size_t i = 1; i <= c.size(); i++)
	{
		if (c[i].GetIdx() != i) return false;
	}
	return true;
};

SCENARIO("Overload resolution regarding CreatePost/Back and Insert and their r-value overloads is correct", tag)
{
	GIVEN("A default constructed PostContainer")
	{
		PostContainer container;

		WHEN("Lvalues and Rvalues of std::string or sb::Post are used to call CreatePost/Back and Insert")
		{
			Post lvalue_post("Testing");
			Post simulating_rvalue_post("Testing 2");

			string lvalue_str = "More testing";
			string simulating_rvalue_str = "More testing 2";
			string simulating_rvalue_str2 = "More testing 3";

			container.Insert(container.IteratorFromIndex(1), lvalue_post);
			container.Insert(container.IteratorFromIndex(2), std::move(simulating_rvalue_post));

			container.CreatePost(container.IteratorFromIndex(1), lvalue_str);
			container.CreatePost(container.IteratorFromIndex(1), std::move(simulating_rvalue_str));

			container.CreatePostBack(lvalue_str);
			container.CreatePostBack(std::move(simulating_rvalue_str2));

			THEN("Lvalues are used to copy construct a Post, rvalues suffer a move operation")
			{
				REQUIRE_FALSE(lvalue_post.content.Empty());
				REQUIRE(simulating_rvalue_post.content.Empty());

				REQUIRE_FALSE(lvalue_str.empty());
				REQUIRE(simulating_rvalue_str.empty());
				REQUIRE(simulating_rvalue_str2.empty());

				REQUIRE(container[1].content[1] == "More testing 2");
				REQUIRE(container[2].content[1] == "More testing");
				REQUIRE(container[3].content[1] == "Testing");
				REQUIRE(container[4].content[1] == "Testing 2");
				REQUIRE(container[5].content[1] == "More testing");
				REQUIRE(container[6].content[1] == "More testing 3");

			}
		}
	}
}

SCENARIO("IteratorFromIndex returns valid iterators or throws on invalid indexes", tag)
{
	GIVEN("A default constructed PostContainer")
	{
		PostContainer container;
		WHEN("IteratorFromIndex is called on Index 0")
		{
			THEN("An exception is thrown")
			{
				REQUIRE_THROWS_WITH(container.IteratorFromIndex(0), "PostContainer: LuaVector does not have index 0.");
			}
		}
		AND_WHEN("IteratorFromIndex is called on empty container")
		{
			THEN("it always throws")
			{
				REQUIRE_THROWS_AS(container.IteratorFromIndex(-1), utils::PostContainerError);
				REQUIRE_NOTHROW(container.IteratorFromIndex(1) == container.end());
				REQUIRE_THROWS_WITH(container.IteratorFromIndex(5), "PostContainer: LuaVector does not have index 5.");
				REQUIRE_THROWS_WITH(container.IteratorFromIndex(10), "PostContainer: LuaVector does not have index 10.");
			}
		}
		AND_WHEN("Posts are added to container")
		{
			container.CreatePostBack("Sample post.");
			container.CreatePostBack("Sample post 2.");
			container.CreatePostBack("Sample post 3.");
			container.CreatePostBack("Sample post 4.");
			THEN("IteratorFromIndex returns valid iterators")
			{
				auto& post1_content = (*container.IteratorFromIndex(1)).content[1];
				auto& post2_content = (*container.IteratorFromIndex(2)).content[1];
				auto& post3_content = (*container.IteratorFromIndex(3)).content[1];
				auto& post4_content = (*container.IteratorFromIndex(4)).content[1];
				REQUIRE_NOTHROW(container.IteratorFromIndex(5) == container.end());

				REQUIRE(post1_content == "Sample post.");
				REQUIRE(post2_content == "Sample post 2.");
				REQUIRE(post3_content == "Sample post 3.");
				REQUIRE(post4_content == "Sample post 4.");
			}
		}
	}
}

SCENARIO("Creating Post in-place works", tag)
{
	GIVEN("A default constructed PostContainer")
	{
		PostContainer container;
		
		WHEN("CreatePost/Back is called")
		{
			std::string emulating_rvalue = "Content";

			container.CreatePostBack(std::move(emulating_rvalue));

			THEN("A move construction occurs with the arguments passed to the function")
			{
				REQUIRE(emulating_rvalue.empty());

				Post& new_post = container[1];

				REQUIRE(new_post.content[1] == "Content");
			}
		}
	}
}

SCENARIO("Erasing posts updates container's contents indexes", tag)
{
	GIVEN("A PostContainer with some posts, some of which have links")
	{
		const string sample_board = R"({
          { -- ID = 1
            tags = {
              connects_to = {
                2
              },
              falsehood = true,
              truth = {false},
              maybe ={false, true},
              random = {5, 7, 8, 50.5}

            },
            content = {"Nothing at all.", "Testing content."}
          },
          { -- ID = 2
            tags = {connects_to = 3},
            content = {"Getting your bearings.",}
          },
          { -- ID = 3
            tags = {
				connects_to = 5,
              sometext = "Random text.",
              on_top = true,
          },
            content = "Playing around with testing."
          },
		  { -- ID = 4
            content = {"Does not mean anything.",}
          },
          { -- ID = 5
			tags = {connects_to = {2, 4}},
            content = {"Getting your bearings.",}
          },
        })";
		LuaStack::Init();
		BoardParser parser;
		PostContainer container = parser.Parse(LuaStack::DeserializeTableString(sample_board));

		WHEN("Indexes are changed because of a removal")
		{
			auto& post3 = container[3];
			REQUIRE(post3.GetIdx() == 3);

			auto& post5 = container[5];
			REQUIRE(post5.GetIdx() == 5);

			container.Erase(container.IteratorFromIndex(4));

			THEN("Posts stay connected, and the ID they hold of connected post is updated")
			{
				REQUIRE(post3.GetIdx() == 3);
				auto& post3_connections = post3.tags["connects_to"];
				REQUIRE((post3_connections.size() == 1 && post3_connections[1] == 4));
				
			}
		}
		WHEN("A post that is connected to another is removed")
		{
			auto& post1 = container[1];
			REQUIRE(post1.GetIdx() == 1);
			REQUIRE(post1.tags.HasConnection());
			auto& post1_connections = post1.tags["connects_to"];
			REQUIRE((post1_connections.size() == 1 && post1_connections[1] == 2));

			auto& post3 = container[3];
			REQUIRE(post3.GetIdx() == 3);
			REQUIRE(post3.tags.HasConnection());
			auto& post3_connections = post3.tags["connects_to"];
			REQUIRE((post3_connections.size() == 1 && post3_connections[1] == 5));

			auto& post5 = container[5];
			REQUIRE(post5.GetIdx() == 5);
			REQUIRE(post5.tags.HasConnection());
			auto& post5_connections = post5.tags["connects_to"];
			REQUIRE(post5_connections.size() == 2);
			REQUIRE((post5_connections[1] == 2 && post5_connections[2] == 4));

			container.Erase(container.IteratorFromIndex(2));

			THEN("Connections to the erased post are deleted, and connections that were invalidated by index changes are remade")
			{
				REQUIRE_FALSE(post1.tags.HasConnection());

				REQUIRE((post3_connections.size() == 1 && post3_connections[1] == 4));

				REQUIRE((post5_connections.size() == 1));
				REQUIRE(post5_connections[1] == 3);
			}

			
		}
	}
}

SCENARIO("Calling MoveToLastPosition correctly adjusts indexes", tag)
{
	GIVEN("A PostContainer populated with nodes that hold connections")
	{
		const string sample_board = R"({
          { -- ID = 1
            tags = {
              connects_to = {
                2
              },
              falsehood = true,
              truth = {false},
              maybe ={false, true},
              random = {5, 7, 8, 50.5}

            },
            content = {"Nothing at all.", "Testing content."}
          },
          { -- ID = 2
            tags = {connects_to = 3},
            content = {"Getting your bearings.",}
          },
          { -- ID = 3
            tags = {
				connects_to = 5,
              sometext = "Random text.",
              on_top = true,
          },
            content = "Playing around with testing."
          },
		  { -- ID = 4
            content = {"Does not mean anything.",}
          },
          { -- ID = 5
			tags = {connects_to = {2, 4}},
            content = {"Getting your bearings.",}
          },
        })";
		LuaStack::Init();
		PostContainer container = BoardParser().Parse(LuaStack::DeserializeTableString(sample_board));

		REQUIRE(container.size() == 5);
		REQUIRE(AreIndexesValid(container));

		const Post& post1 = container[1];
		const auto& post1_connections = post1.tags["connects_to"];
		REQUIRE(post1_connections.size() == 1);
		REQUIRE(post1_connections[1] == 2);

		container.MoveToLastPosition(container.begin() + 1);

		REQUIRE(AreIndexesValid(container));

		const Post& post1_new = container[1];
		const auto& post1_connections_new = post1_new.tags["connects_to"];
		REQUIRE(post1_connections_new.size() == 1);

		REQUIRE(post1_connections_new[1] == container.size());

		const Post& post4 = container[4];
		const auto& post4_connections = post4.tags["connects_to"];

		REQUIRE(post4_connections.size() == 2);
		REQUIRE(post4_connections[1] == 5);
		REQUIRE(post4_connections[2] == 3);
	}
}

SCENARIO("Any change on the order of the container's elements is reflected on their indexes", tag)
{
	GIVEN("A PostContainer holding some posts")
	{
		const string sample_board = R"({
          { -- ID = 1
            tags = {
              connects_to = {
                2
              },
              falsehood = true,
              truth = {false},
              maybe ={false, true},
              random = {5, 7, 8, 50.5}

            },
            content = {"Nothing at all.", "Testing content."}
          },
          { -- ID = 2
            tags = {connects_to = 3},
            content = {"Getting your bearings.",}
          },
          { -- ID = 3
            tags = {
				connects_to = 5,
              sometext = "Random text.",
              on_top = true,
          },
            content = "Playing around with testing."
          },
		  { -- ID = 4
            content = {"Does not mean anything.",}
          },
          { -- ID = 5
			tags = {connects_to = {2, 4}},
            content = {"Getting your bearings.",}
          },
        })";
		LuaStack::Init();
		PostContainer container = BoardParser().Parse(LuaStack::DeserializeTableString(sample_board));
		REQUIRE(container.size() == 5);

		WHEN("A post is removed")
		{
			REQUIRE_NOTHROW(container.Erase(container.IteratorFromIndex(2)));

			THEN("All the posts' indexes are updated and make sense")
			{
				REQUIRE(container.size() == 4);
				REQUIRE(AreIndexesValid(container));
				REQUIRE_FALSE(container[1].tags.HasConnection());
			}
		}
	}
}

SCENARIO("Comparisons using operator== are reliable", tag)
{
	GIVEN("two PostContainers")
	{
		PostContainer c1, c2;

		WHEN("The == operator is called")
		{
			THEN("The comparisons are always accurate")
			{
				REQUIRE(c1 == c2);
				c1.CreatePostBack("Flanders!");
				REQUIRE_FALSE(c1 == c2);
				c2.CreatePostBack("Flanders!");
				REQUIRE(c1 == c2);
				c2[1].tags["random"].EmplaceBack("Abcdefg");
				REQUIRE_FALSE(c1 == c2);
			}
		}
	}
}

SCENARIO("Empty() is reliable", tag)
{
	GIVEN("A default constructed PostContainer")
	{
		PostContainer container;
		WHEN("Empty is called right after default construction")
		{
			THEN("Empty returns true")
			{
				REQUIRE(container.Empty());
			}
		}
		WHEN("Posts are inserted on the container")
		{
			container.CreatePostBack("Testing Empty");
			container.CreatePostBack("Testing Empty 2");
			
			THEN("Empty returns false")
			{
				REQUIRE_FALSE(container.Empty());
			}
		}
		WHEN("Posts are deleted from container")
		{
			container.CreatePostBack("Testing Empty");
			container.CreatePostBack("Testing Empty 2");
			container.Erase(container.IteratorFromIndex(2));
			container.PopBack();
			
			
			THEN("Empty returns true")
			{
				REQUIRE(container.Empty());
			}
		}
	}
}

SCENARIO("The resizing operation works to specifications", tag)
{
	GIVEN("A default constructed PostContainer")
	{
		PostContainer container;

		REQUIRE(container.Empty());

		REQUIRE_THROWS_WITH(container[1], "LuaVector does not have index 1.");

		WHEN("Resize is called")
		{
			REQUIRE_NOTHROW(container.Resize(1));

			THEN("Size of inner LuaVector expands")
			{
				REQUIRE(container.size() == 1);

				REQUIRE(container[1].content[1] == "");

				REQUIRE_NOTHROW(container[1] = Post("Nothing."));

				REQUIRE(container[1].content[1] == "Nothing.");

				container.Resize(10);

				REQUIRE(container.size() == 10);

				container.Resize(5);

				REQUIRE(container.size() == 5);

				REQUIRE_THROWS(container.Resize(-50));
			}
		}
	}
}