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
		const string sample_board = R"({posts = 
{
  {
    color = {
      150,
      150,
      150
    },
    content = {
      "Nothing at all.",
      "Testing content."
    },
    display_pos = {
      50,
      50
    },
    tags = {
      falsehood = {
        true
      },
      maybe = {
        false,
        true
      },
      next = {
        2
      },
      random = {
        5,
        7,
        8,
        50.5
      },
      truth = {
        false
      }
    }
  },
  {
    content = {
      "Getting your bearings."
    },
    display_pos = {
      150,
      150
    }
  },
  {
    content = {
      "Playing around with testing."
    },
    display_pos = {
      311,
      295
    },
    tags = {
      on_top = {
        true
      },
      sometext = {
        "Random text."
      }
    }
  },
  {
    content = {
      "Getting your bearings."
    },
    display_pos = {
      250,
      250
    }
  }
},
board_config = {bg_color = {30, 30, 30}}
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
				REQUIRE(false);
			}
		}
		WHEN("A post that is connected to another is removed")
		{
			REQUIRE(false);

			THEN("Connections to the erased post are deleted, and connections that were invalidated by index changes are remade")
			{
				REQUIRE(false);
			}

			
		}
	}
}

SCENARIO("Calling MoveToLastPosition correctly adjusts indexes", tag2)
{
	GIVEN("A PostContainer populated with nodes that hold connections")
	{
		const string sample_board = R"({posts = 
{
  { -- 1
    color = {
      150,
      150,
      150
    },
    content = {
      "Nothing at all.",
      "Testing content."
    },
    display_pos = {
      50,
      50
    },
    tags = {
      falsehood = {
        true
      },
      maybe = {
        false,
        true
      },
      connects_to = {
        2
      },
      random = {
        5,
        7,
        8,
        50.5
      },
      truth = {
        false
      }
    }
  },
  { -- 2
    content = {
      "Getting your bearings."
    },
    display_pos = {
      150,
      150
    }
  },
  { -- 3
    content = {
      "Playing around with testing."
    },
    display_pos = {
      311,
      295
    },
    tags = {
      on_top = {
        true
      },
	  connects_to = {2, 3},
      sometext = {
        "Random text."
      }
    }
  },
  { -- 4
    content = {
      "Getting your bearings."
    },
    display_pos = {
      250,
      250
    }
  }
},
board_config = {bg_color = {30, 30, 30}}
})";
		LuaStack::Init();
		PostContainer container = BoardParser().Parse(LuaStack::DeserializeTableString(sample_board));

		REQUIRE(container.size() == 4);
		REQUIRE(AreIndexesValid(container));

		const Post& post1 = container[1];
		const auto& post1_connections = post1.tags["connects_to"];
		REQUIRE(post1_connections.size() == 1);
		REQUIRE(post1_connections[1] == 2);

		container.MoveToLastPosition(container.begin() + 1);

		REQUIRE(AreIndexesValid(container));

		REQUIRE(false);
	}
}

SCENARIO("Any change on the order of the container's elements is reflected on their indexes", tag2)
{
	GIVEN("A PostContainer holding some posts")
	{
		const string sample_board = R"({posts = 
{
  {
    color = {
      150,
      150,
      150
    },
    content = {
      "Nothing at all.",
      "Testing content."
    },
    display_pos = {
      50,
      50
    },
    tags = {
      falsehood = {
        true
      },
      maybe = {
        false,
        true
      },
      connects_to = {
        2
      },
      random = {
        5,
        7,
        8,
        50.5
      },
      truth = {
        false
      }
    }
  },
  {
    content = {
      "Getting your bearings."
    },
    display_pos = {
      150,
      150
    }
  },
  {
    content = {
      "Playing around with testing."
    },
    display_pos = {
      311,
      295
    },
    tags = {
      on_top = {
        true
      },
      sometext = {
        "Random text."
      }
    }
  },
  {
    content = {
      "Getting your bearings."
    },
    display_pos = {
      250,
      250
    }
  }
},
board_config = {bg_color = {30, 30, 30}}
})";
		LuaStack::Init();
		PostContainer container = BoardParser().Parse(LuaStack::DeserializeTableString(sample_board));
		REQUIRE(container.size() == 4);
		REQUIRE(AreIndexesValid(container));
		REQUIRE(container[1].tags.HasConnection());
		REQUIRE(container[1].tags["connects_to"].size() == 1);
		REQUIRE(container[1].tags["connects_to"][1] == 2);

		WHEN("A post is removed")
		{
			REQUIRE_NOTHROW(container.Erase(container.IteratorFromIndex(2)));

			THEN("All the posts' indexes are updated and make sense")
			{
				REQUIRE(container.size() == 3);
				REQUIRE(AreIndexesValid(container));
				REQUIRE_FALSE(container[1].tags.HasConnection());
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