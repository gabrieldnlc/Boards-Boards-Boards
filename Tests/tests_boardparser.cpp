#include <string>

#include "catch.hpp"

#include "utils/parsing/BoardParser.hpp"
#include "containers/PostContainer.hpp"
#include "utils/LuaStack.hpp"

using utils::LuaStack;
using utils::BoardParser;
using sb::PostContainer;
using std::string;

const string tag = "[BoardParser]";

SCENARIO("A serialized PostContainer is correctly deserialized into a PostContainer", tag)
{
    GIVEN("a default constructed BoardParser")
    {
        LuaStack::Init();

        BoardParser parser;

        WHEN("Parse is called on a valid script")
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

            sol::table deserialized = LuaStack::DeserializeTableString(sample_board);

            PostContainer posts;
            PostContainer another = parser.Parse(deserialized);

            REQUIRE_NOTHROW(posts = (parser.Parse(deserialized)));

            THEN("Parse is executed to the specifications")
            {

                REQUIRE_THROWS_WITH(posts[0], "LuaVector does not have index 0.");
                REQUIRE_THROWS_WITH(posts[5], "LuaVector does not have index 5.");

                auto& p1 = posts[1];                
                auto& p1_tags = p1.tags;
                REQUIRE(p1_tags.HasTag("next"));
                REQUIRE(p1_tags.HasTag("falsehood"));
                REQUIRE(p1_tags.HasTag("truth"));
                REQUIRE(p1_tags.HasTag("maybe"));
                REQUIRE(p1_tags.HasTag("random"));
                REQUIRE_FALSE(p1_tags.HasTag("inexistent"));
                REQUIRE(p1_tags["next"][1] == 2);
                REQUIRE(p1_tags["falsehood"][1] == true);
                REQUIRE(p1_tags["maybe"][1] == false);
                REQUIRE(p1_tags["maybe"][2] == true);
                REQUIRE(p1_tags["random"][1] == 5);
                REQUIRE(p1_tags["random"][2] == 7);
                REQUIRE(p1_tags["random"][3] == 8);
                REQUIRE(p1_tags["random"][4] == 50.5f);
                REQUIRE(p1.content[1] == "Nothing at all.");
                REQUIRE(p1.content[2] == "Testing content.");

                auto& p2 = posts[2];
                REQUIRE(p2.tags.Empty());
                REQUIRE(p2.content[1] == "Getting your bearings.");

                auto& p3 = posts[3];
                auto& p3_tags = p3.tags;
                REQUIRE(p3_tags.HasTag("sometext"));
                REQUIRE(p3_tags["sometext"][1] == "Random text.");
                REQUIRE(p3_tags.HasTag("on_top"));
                REQUIRE(p3_tags["on_top"][1] == true);
                REQUIRE(p3.content[1] == "Playing around with testing.");

                auto& p4 = posts[4];
                REQUIRE(p4.tags.Empty());
                REQUIRE(p4.content[1] == "Getting your bearings.");

            }
        }
    }    
}

SCENARIO("Attempting to deserialize an invalid script will throw", tag)
{
    GIVEN("a default constructed BoardParser")
    {
        LuaStack::Init();

        BoardParser parser;

        WHEN("Parse is called on a malformed script")
        {
            const string no_posts = R"({
  posts__ = {
    {
      content = {
        "Getting your bearings."
      },
      display_pos = {
        150,
        150
      }
    }
  }
})";

            const string not_valid = R"({{}})";

            const string post_not_valid = R"({
  posts = {
    {
      content___ = {
        "Getting your bearings."
      },
      display_pos = {
        150,
        150
      }
    }
  }
})";

            THEN("exceptions are thrown")
            {
                REQUIRE_THROWS_WITH(parser.Parse(LuaStack::DeserializeTableString(no_posts)), "The file does not have a Posts table.");
                REQUIRE_THROWS_WITH(parser.Parse(LuaStack::DeserializeTableString(not_valid)), "The file does not have a Posts table.");
                REQUIRE_THROWS_WITH(parser.Parse(LuaStack::DeserializeTableString(post_not_valid)), "Post has no valid 'content' field.");
            }
        }
    }
}

SCENARIO("A deserialized PostContainer is equal to the original", tag)
{
    GIVEN("A sample PostContainer")
    {
        LuaStack::Init();

        BoardParser parser;

        const string sample_board = R"({
          posts = {
            tags = {
              next = {
                2
              },
              falsehood = true,
              truth = {false},
              maybe ={false, true},
              random = {5, 7, 8, 50.5}
            },
            content = {"Nothing at all.", "Testing content."},
            display_pos = {50, 50},
            color = {150, 150, 150},
          },
          {
            tags = {},
            content = {"Getting your bearings.",},
            display_pos = {150, 150},
          },
          {
            tags = {
              sometext = "Random text.",
              on_top = true,
          },
            content = "Playing around with testing.",
            display_pos = {200, 200},
          },
          {
            content = {"Getting your bearings.",},
            display_pos = {250, 250},
          },
        })";

        sol::table deserialized = LuaStack::DeserializeTableString(sample_board);

        PostContainer posts;

        REQUIRE_NOTHROW(posts = std::move(parser.Parse(deserialized)));

        sol::table serialized = parser.ToTable(posts);

        PostContainer other_posts = parser.Parse(serialized);

        REQUIRE(posts == other_posts);
    }
}