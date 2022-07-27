#include <string>

#include "catch.hpp"

#include "renderables/posts/Tags.hpp"

using namespace board;

const std::string t1 = "[Tag]";
const std::string t2 = "[LuaValue]";

//NODE TAGS TESTS

SCENARIO("removing entry via value or index works as specified", t1)
{
	GIVEN("a Tags instance with some tags set")
	{
		Tags t;
		t["connects_to"].PushBack(5);
		t["unlocks_node"].PushBack(9);
		t["unlocks_node"].PushBack(10);
		
		Tags t2 = t;
		REQUIRE(t == t2);

		THEN("RemoveFromKey works as specified")
		{
			REQUIRE(t2.HasTag("unlocks_node"));
			REQUIRE(t2["unlocks_node"].size() == 2);

			REQUIRE_FALSE(t2.RemoveFromKey("prev", 8));
			REQUIRE_FALSE(t2.RemoveFromKey("connects_to", 8));
			REQUIRE(t2["connects_to"].size() == 1);

			REQUIRE_FALSE(t2.RemoveFromKey("unlocks_node", 8));
			REQUIRE(t2["unlocks_node"].size() == 2);
			REQUIRE(t == t2);

			REQUIRE(t2.RemoveFromKey("unlocks_node", 9));
			REQUIRE(t2.HasTag("unlocks_node"));
			REQUIRE(t2["unlocks_node"].size() == 1);
			REQUIRE_FALSE(t == t2);

			REQUIRE(t2.RemoveFromKey("unlocks_node", 10));
			REQUIRE_FALSE(t2.HasTag("unlocks_node"));
		}
		THEN("RemoveIndexFromKey works as specified")
		{
			REQUIRE_FALSE(t2.RemoveIndexFromKey("prev", 0));

			REQUIRE_THROWS_AS(t2.RemoveIndexFromKey("connects_to", -1), std::out_of_range);
			REQUIRE_THROWS_AS(t2.RemoveIndexFromKey("connects_to", 1), std::out_of_range);
			REQUIRE(t2.RemoveIndexFromKey("connects_to", 0));
			REQUIRE_FALSE(t2.HasTag("connects_to"));

			auto& unlocks = t2["unlocks_node"];

			unlocks.PushBack(17);
			unlocks.PushBack(22);

			REQUIRE(unlocks.size() == 4);

			REQUIRE(unlocks[1] == 9);
			REQUIRE(unlocks[2] == 10);
			REQUIRE(unlocks[3] == 17);
			REQUIRE(unlocks[4] == 22);

			REQUIRE(t2.RemoveIndexFromKey("unlocks_node", 2));

			REQUIRE(unlocks.size() == 3);

			REQUIRE(unlocks[1] == 9);
			REQUIRE(unlocks[2] == 10);
			REQUIRE(unlocks[3] == 22);
			REQUIRE_THROWS(unlocks.at(4));
			
		}
			
	}
}

SCENARIO("HasConnection reports correctly", t1)
{
	GIVEN("a default constructed Tags instance")
	{
		Tags t;
		WHEN("tag connection is updated or deleted")
		{
			THEN("HasConnection reports are accurate")
			{
				REQUIRE_FALSE(t.HasConnection());
				t["connects_to"];
				REQUIRE_FALSE(t.HasConnection());
				t["connects_to"].PushBack(5);
				REQUIRE(t.HasConnection());
				REQUIRE(t.RemoveFromKey("connects_to", 5));
				REQUIRE_FALSE(t.HasConnection());
			}
		}
	}
}

SCENARIO("operator[] will create entry if key does not exist", t1)
{
	GIVEN("an empty Tags instance")
	{
		Tags t;
		WHEN("operator[] is called")
		{
			THEN("if key does not exist, it is created")
			{
				REQUIRE_FALSE(t.HasTag("connects_to"));
				t["connects_to"];
				REQUIRE(t.HasTag("connects_to"));
			}
			AND_THEN("a operator[] call on const passes, but throws if it would create key")
			{
				t["connects_to"];
				const auto& t2 = t;
				REQUIRE_NOTHROW(t2["connects_to"]);
				REQUIRE_THROWS(t2["does not exist"]);
			}
		}
	}
}

SCENARIO("operator== correctfully compares two NodeTag instances", t1)
{
	GIVEN("two default constructed NodeTag instances")
	{
		Tags t1;
		Tags t2;
		WHEN("the contents of each are changed")
		{
			THEN("operator== correctfully compares")
			{
				REQUIRE(t1 == t2);

				t1["connects_to"];
				REQUIRE_FALSE(t1 == t2);
				t2["connects_to"];
				REQUIRE(t1 == t2);
				t1["connects_to"].PushBack(5);
				t2["connects_to"].PushBack(5);
				REQUIRE(t1 == t2);
				t1["connects_to"].PushBack(6);
				REQUIRE_FALSE(t1 == t2);
				t1["connects_to"].PopBack();
				REQUIRE(t1 == t2);
				t1["falsehood"].PushBack(true);
				REQUIRE_FALSE(t1 == t2);
				t2["falsehood"].PushBack(true);
				REQUIRE(t1 == t2);
				t1["connects_to"].Clear();
				t2["connects_to"].Clear();
				REQUIRE(t1 == t2);
				t1["connects_to"].PushBack(5);
				t2["connects_to"].PushBack(5.0f);
				REQUIRE_FALSE(t1 == t2);
				t1["connects_to"].Clear();
				t2["connects_to"].Clear();
				t1["connects_to"].PushBack(5);
				t2["connects_to"].PushBack(6);
				REQUIRE_FALSE(t1 == t2);
				t1["connects_to"].PushBack(5);
				REQUIRE_FALSE(t1 == t2);
				
			}
		}
	}
}

//TAG ENTRY TESTS
SCENARIO("default constructor results on int 0", t2)
{
	GIVEN("a default constructed LuaValue")
	{
		LuaValue e;
		THEN("it must be an int 0")
		{
			REQUIRE(e.isInt());
			REQUIRE(e.asInt() == 0);
		}
	}
}

SCENARIO("constructor correctly accepts valid arguments", t2)
{
	GIVEN("constructor arguments of int, float, bool or string")
	{
		WHEN("constructor is called")
		{
			LuaValue e1(5);
			LuaValue e2(6.0f);
			LuaValue e3(true);
			LuaValue e4("made from a char*");
			LuaValue e5(std::string("made from a string variable"));
			LuaValue e6(std::size_t(8));
			LuaValue e7("true");
			LuaValue e8("false");



			THEN("entry is sucessfully created")
			{
				REQUIRE(e1.isInt());
				REQUIRE(e1.asInt() == 5);

				REQUIRE(e2.isFloat());
				REQUIRE(e2.asFloat() == 6.0f);

				REQUIRE(e3.isBool());
				REQUIRE(e3.asBool() == true);

				REQUIRE(e4.isString());
				REQUIRE(e4.asString() == "made from a char*");

				REQUIRE(e5.isString());
				REQUIRE(e5.asString() == "made from a string variable");

				REQUIRE(e6.isInt());
				REQUIRE(e6.asInt() == 8);

				REQUIRE_FALSE(e7.isString());
				REQUIRE_FALSE(e8.isString());
				REQUIRE(e7.isBool());
				REQUIRE(e7.asBool() == true);
				REQUIRE(e8.isBool());
				REQUIRE(e8.asBool() == false);
			}
		}
		WHEN("string constructor is called with two arguments")
		{
			THEN("string is correctly parsed or throws on impossibilities")
			{
				LuaValue e1;

				REQUIRE_THROWS(e1 = LuaValue("anything", -1));
				REQUIRE_THROWS_WITH(e1 = LuaValue("anything", 4), "A Tag cannot be initialized with index 4.");
				
				e1 = LuaValue("5", 0);
				REQUIRE(e1.isInt());
				REQUIRE_NOTHROW(e1.asInt());
				REQUIRE_THROWS_AS(e1.asFloat(), std::bad_variant_access);

				LuaValue e2("6", 1);
				REQUIRE(e2.isFloat());
				REQUIRE_NOTHROW(e2.asFloat());
				REQUIRE_THROWS_AS(e2.asInt(), std::bad_variant_access);

				LuaValue e3("true", 2);
				LuaValue e4("true");
				LuaValue e5;
				REQUIRE_THROWS_WITH((e5 = LuaValue("not boolean", 2)), "Could not parse string into boolean Tag.");
				REQUIRE(e3.isBool());
				REQUIRE_NOTHROW(e3.asBool());
				REQUIRE_THROWS_AS(e2.asInt(), std::bad_variant_access);
				REQUIRE(e4.isBool());
				REQUIRE_NOTHROW(e4.asBool());
				REQUIRE_THROWS_AS(e4.asString(), std::bad_variant_access);

				e5 = LuaValue("made from a char*", 3);
				LuaValue e6(std::string("made from a string variable"), 3);

				REQUIRE(e5.isString());
				REQUIRE(e6.isString());

				REQUIRE_NOTHROW(e5.asString());
				REQUIRE_NOTHROW(e6.asString());

			}
		}
	}
}

SCENARIO("operator= correctly constructs", t2)
{
	GIVEN("int, float, bool or string types")
	{
		WHEN("operator= is called")
		{
			THEN("the result is a valid and correctly assigned LuaValue")
			{
				LuaValue e1 = 5;
				LuaValue e2 = 6.0f;
				LuaValue e3 = false;
				LuaValue e4 = "made from a char*";
				LuaValue e5 = std::string("made from a string variable");

				REQUIRE(e1.isInt());
				REQUIRE(e1.asInt() == 5);

				REQUIRE(e2.isFloat());
				REQUIRE(e2.asFloat() == 6.0f);

				REQUIRE(e3.isBool());
				REQUIRE(e3.asBool() == false);

				REQUIRE(e4.isString());
				REQUIRE(e4.asString() == "made from a char*");

				REQUIRE(e5.isString());
				REQUIRE(e5.asString() == "made from a string variable");
			}
		}
	}
}

SCENARIO("operator= correctly assigns", t2)
{
	GIVEN("int, float, bool or string variables")
	{
		int a = 10;
		float b = 55.f;
		bool c = true;
		const char* d = "made from a char*";
		std::string e = "made from a string variable";

		WHEN("operator= is called on existing LuaValue")
		{
			LuaValue e1;
			LuaValue e2;
			LuaValue e3;
			LuaValue e4;
			LuaValue e5;

			e1 = a;
			e2 = b;
			e3 = c;
			e4 = d;
			e5 = e;

			THEN("the result must be a valid LuaValue")
			{
				REQUIRE(e1.isInt());
				REQUIRE(e1.asInt() == 10);

				REQUIRE(e2.isFloat());
				REQUIRE(e2.asFloat() == 55.f);

				REQUIRE(e3.isBool());
				REQUIRE(e3.asBool() == true);

				REQUIRE(e4.isString());
				REQUIRE(e4.asString() == "made from a char*");

				REQUIRE(e5.isString());
				REQUIRE(e5.asString() == "made from a string variable");
			}
		}
	}
}

SCENARIO("as___ functions will throw if call does not correspond to type", t2)
{
	GIVEN("initialized TagEntries")
	{
		LuaValue e1 = 5;
		LuaValue e2 = 6.0f;
		LuaValue e3 = false;
		LuaValue e4 = "made from a char*";
		LuaValue e5 = std::string("made from a string variable");

		WHEN("a mismatched call to an as___ function is made")
		{
			THEN("exception is thrown")
			{
				REQUIRE_NOTHROW(e1.asInt());
				REQUIRE_THROWS_AS(e1.asFloat(), std::bad_variant_access);

				REQUIRE_NOTHROW(e2.asFloat());
				REQUIRE_THROWS_AS(e2.asInt(), std::bad_variant_access);

				REQUIRE_NOTHROW(e3.asBool());
				REQUIRE_THROWS_AS(e3.asInt(), std::bad_variant_access);

				REQUIRE_NOTHROW(e4.asString());
				REQUIRE_THROWS_AS(e4.asFloat(), std::bad_variant_access);

				REQUIRE_NOTHROW(e5.asString());
				REQUIRE_THROWS_AS(e5.asBool(), std::bad_variant_access);
			}
		}
	}
}

SCENARIO("operator== correctly compares with raw types", t2)
{
	GIVEN("initialized TagEntries")
	{
		LuaValue e1 = 5;
		LuaValue e2 = 6.0f;
		LuaValue e3 = false;
		LuaValue e4 = "made from a char*";
		LuaValue e5 = std::string("made from a string variable");
		
		WHEN("operator== is called")
		{
			THEN("it safely compares and results make sense")
			{
				REQUIRE(e1 == 5);
				REQUIRE(e1 == 5.f);
				REQUIRE_FALSE(e1 == 6);

				REQUIRE(e2 == 6.0f);
				REQUIRE(e2 == 6);
				REQUIRE_FALSE(e2 == 5.f);

				REQUIRE(e3 == false);
				REQUIRE_FALSE(e3 == 0);
				REQUIRE_FALSE(e3 == 1);
				REQUIRE_FALSE(e3 == "false");

				REQUIRE(e4 == "made from a char*");
				REQUIRE_FALSE(e4 == "made from something else");
				REQUIRE_FALSE(e4 == 0);
				REQUIRE_FALSE(e4 == "");
				REQUIRE_FALSE(e4 == true);

				REQUIRE(e5 == "made from a string variable");
				REQUIRE_FALSE(e5 == true);
				REQUIRE_FALSE(e5 == "made from something else");
				REQUIRE_FALSE(e5 == 0);
				REQUIRE_FALSE(e5 == 10.f);
			}
		}
	}
}

SCENARIO("operator== correctly compares two Tag Entries", t2)
{
	GIVEN("initialized TagEntries")
	{
		LuaValue int1 = 5;
		LuaValue int2 = 10;
		LuaValue float1 = 6.0f;
		LuaValue float2 = 15.0f;
		LuaValue bool1 = true;
		LuaValue bool2 = false;
		LuaValue string1 = "made from a char*";
		LuaValue string2 = std::string("made from a string variable");
		WHEN("operator== is called on entries of the same underlying type")
		{
			THEN("the comparisons are always accurate")
			{
				REQUIRE_FALSE(int1 == int2);
				int2 = 5;
				REQUIRE(int1 == int2);
				int2 = 15.0f;
				REQUIRE_FALSE(int1 == int2);
				int2 = 5.f;
				REQUIRE_FALSE(int1 == int2);

				REQUIRE_FALSE(float1 == float2);
				float2 = 6.0f;
				REQUIRE(float1 == float2);
				float2 = 10.f;
				REQUIRE_FALSE(float1 == float2);
				float2 = 6;
				REQUIRE_FALSE(float1 == float2);

				REQUIRE_FALSE(bool1 == bool2);
				bool2 = true;
				REQUIRE(bool1 == bool2);

				REQUIRE_FALSE(string1 == string2);
				string2 = "made from a char*";
				REQUIRE(string1 == string2);
				string1 = std::string("Test string");
				string2 = "Test string";
				REQUIRE(string1 == string2);
			}
		}

		WHEN("operator== is called on entries of different underlying types")
		{
			THEN("comparison of different types will always fail")
			{
				REQUIRE_FALSE(float1 == bool1);
				string1 = "";
				int1 = 0;
				REQUIRE_FALSE(int1 == string1);
				int1 = 6;
				string1 = "6";
				REQUIRE_FALSE(int1 == string1);
				string1 = "7";
			}
			AND_THEN("comparison between ints and floats will never return true")
			{
				REQUIRE_FALSE(int1 == float1);
				float1 = 5.0f;
				REQUIRE_FALSE(int1 == float1);
				int1 = 10;
				REQUIRE_FALSE(int1 == float1);
			}
		}
	}
}

SCENARIO("entry.label is always accurate", t2)
{
	GIVEN("initialized TagEntries")
	{
		LuaValue int1 = 5;
		LuaValue int2 = 10;
		LuaValue float1 = 6.0f;
		LuaValue float2 = 15.65f;
		LuaValue bool1 = true;
		LuaValue bool2 = false;
		LuaValue string1 = "made from a char*";
		LuaValue string2 = std::string("made from a string variable");
		THEN("the labels generated during object construction are accurate to their value")
		{
			REQUIRE(int1.GetLabel() == "5");
			REQUIRE(int2.GetLabel() == "10");
			REQUIRE(float1.GetLabel() == "6.000000");
			REQUIRE(float2.GetLabel() == "15.650000");
			REQUIRE(bool1.GetLabel() == "true");
			REQUIRE(bool2.GetLabel() == "false");
			REQUIRE((string1.GetLabel() == "made from a char*" && string1.GetLabel() == string1.asString()));
			REQUIRE((string2.GetLabel() == "made from a string variable" && string2.GetLabel() == string2.asString()));
		}
	}
}



