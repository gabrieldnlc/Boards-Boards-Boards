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

const string tag = "[Post]";
const string tag2 = "[PostContainer]";

auto AreIndexesValid = [](PostContainer& c) -> bool
{
	for (std::size_t i = 1; i <= c.size(); i++)
	{
		if (c[i].GetIdx() != i) return false;
	}
	return true;
};

SCENARIO("Post.HasColor() reports correctly", tag)
{
	GIVEN("A default constructed Post")
	{
		Post post;
		THEN("It has no color")
		{
			REQUIRE_FALSE(post.HasColor());
		}
		
		WHEN("Not all 3 color fields are >= 0")
		{
			THEN("HasColor() still returns false")
			{
				post.color[0] = 2.f;
				REQUIRE_FALSE(post.HasColor());
				post.color[1] = 3.f;
				REQUIRE_FALSE(post.HasColor());
			}
		}

		WHEN("All 3 color fields are >= 0")
		{
			post.color[0] = 2.f;
			post.color[1] = 3.f;
			post.color[2] = 5.f;

			THEN("HasColor() returns true")
			{
				REQUIRE(post.HasColor());
			}
			AND_WHEN("One of the color fields is changed")
			{
				REQUIRE(post.HasColor());
				THEN("HasColor() returns false")
				{
					post.color[1] = -1.f;
					REQUIRE_FALSE(post.HasColor());
				}
				
			}
		}
	}
}

SCENARIO("Overload resolution regarding CreatePost/Back and Insert and their r-value overloads is correct", tag2)
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

SCENARIO("IteratorFromIndex returns valid iterators or throws on invalid indexes", tag2)
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

SCENARIO("Creating Post in-place works", tag2)
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

SCENARIO("Erasing posts updates container's contents indexes", tag2)
{
	GIVEN("A PostContainer with some posts, some of which have links")
	{
		const string sample_board = R"({
  board_config = {
    bg_color = {
      119,
      119,
      119
    },
    connection_color = {
      255,
      255,
      255
    },
    post_color = {
      35,
      53,
      114
    },
    selected_connection_color = {
      15,
      15,
      15
    },
    text_color = {
      223,
      187,
      187
    }
  },
  connections = {
    {
      1,
      2
    },
    {
      2,
      4
    }
  },
  posts = {
    {
      content = {
        "Testing serialization and deserialization.\n\nThis post has 1 connection."
      },
      display_pos = {
        325,
        251
      }
    },
    {
      color = {
        127,
        20,
        20
      },
      content = {
        "This post is red."
      },
      display_pos = {
        742,
        373
      }
    },
    {
      color = {
        114,
        40,
        204
      },
      content = {
        "But this one is purple."
      },
      display_pos = {
        863,
        285
      }
    },
    {
      content = {
        "This post has a connection, as a treat."
      },
      display_pos = {
        290,
        413
      }
    }
  }
})";
		LuaStack::Init();
		PostContainer container = BoardParser().Parse(LuaStack::DeserializeTableString(sample_board));

		WHEN("Indexes are changed because of a removal")
		{
			REQUIRE(AreIndexesValid(container));

			container.Erase(container.IteratorFromIndex(3));

			REQUIRE(AreIndexesValid(container));

			THEN("Posts stay connected, and the ID they hold of connected post is updated")
			{
				auto& connections = container.connections;
				REQUIRE(connections.size() == 2);

				auto& connection_1 = connections[1];
				auto& connection_2 = connections[2];

				REQUIRE(connection_1.from == 1);
				REQUIRE(connection_1.to == 2);

				REQUIRE(connection_2.from == 2);
				REQUIRE(connection_2.to == 3);
			}
		}
		WHEN("A post that is connected to another is removed")
		{
			REQUIRE(container.size() == 4);
			REQUIRE(container.connections.size() == 2);
			container.Erase(container.IteratorFromIndex(4));
			REQUIRE(container.size() == 3);
			REQUIRE(AreIndexesValid(container));

			THEN("Connections to the erased post are deleted")
			{
				auto& connections = container.connections;
				REQUIRE(connections.size() == 1);
				auto& connection_1 = connections[1];

				REQUIRE(connection_1.from == 1);
				REQUIRE(connection_1.to == 2);
			}

			
		}
	}
}

SCENARIO("Calling MoveToLastPosition correctly adjusts indexes", tag2)
{
	GIVEN("A PostContainer populated with nodes that hold connections")
	{
		const string sample_board = R"({
  board_config = {
    bg_color = {
      119,
      119,
      119
    },
    connection_color = {
      255,
      255,
      255
    },
    post_color = {
      35,
      53,
      114
    },
    selected_connection_color = {
      15,
      15,
      15
    },
    text_color = {
      223,
      187,
      187
    }
  },
  connections = {
    {
      1,
      2
    },
    {
      2,
      3
    }
  },
  posts = {
    {
      content = {
        "Testing serialization and deserialization.\n\nThis post has 1 connection."
      },
      display_pos = {
        325,
        251
      }
    },
    {
      color = {
        127,
        20,
        20
      },
      content = {
        "This post is red."
      },
      display_pos = {
        742,
        373
      }
    },
    {
      color = {
        114,
        40,
        204
      },
      content = {
        "But this one is purple."
      },
      display_pos = {
        863,
        285
      }
    },
    {
      content = {
        "This post has no connections at all."
      },
      display_pos = {
        290,
        413
      }
    }
  }
})";
		LuaStack::Init();
		PostContainer container = BoardParser().Parse(LuaStack::DeserializeTableString(sample_board));

		REQUIRE(AreIndexesValid(container));
		REQUIRE(container.size() == 4);
		REQUIRE(container.connections.size() == 2);

		WHEN("MoveToLastPosition is called on a Post")
		{
			container.MoveToLastPosition(container.IteratorFromIndex(2));
			REQUIRE(container.size() == 4);
			REQUIRE(container[4].content[1].AsString() == "This post is red.");

			THEN("Posts that succeeded the move subject have their indexes decreased by 1")
			{
				REQUIRE(AreIndexesValid(container));
			}
			THEN("Connections to move subject are updated to reflect new indexes")
			{
				auto& connections = container.connections;

				REQUIRE(connections.size() == 2);

				auto& connection_1 = connections[1];
				auto& connection_2 = connections[2];

				REQUIRE(connection_1.from == 1);
				REQUIRE(connection_1.to == 4);
				REQUIRE(connection_2.from == 4);
				REQUIRE(connection_2.to == 2);

			}
		}
	}
}

SCENARIO("Comparisons using operator== are reliable", tag2)
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

SCENARIO("Empty() is reliable", tag2)
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

SCENARIO("The resizing operation works to specifications", tag2)
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