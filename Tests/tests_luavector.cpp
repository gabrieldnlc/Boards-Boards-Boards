#include <string>
#include <memory>

#include "catch.hpp"

#include "containers/LuaVector.hpp"

using namespace sb;
using std::vector;
using std::string;
using std::size_t;

const string tag = "[LuaVector]";

// Testing helpers
template<class T>
bool CompareLuaVectorToVector(LuaVector<T> lvec, vector<T> vec)
{
	if (lvec.size() != vec.size()) return false;
	for (size_t i = 1; i <= lvec.size(); i++)
	{
		if (lvec[i] != vec[i - 1]) return false;
	}
	return true;
}

LuaVector<string> LuaVectorWithStrings(size_t size)
{
	LuaVector<string> v(true);
	for (size_t i = 1; i <= size; i++)
	{
		v[i] = ("Element number " + std::to_string(i));
	}
	return v;
}

SCENARIO("LuaVector has no index 0", tag)
{
	GIVEN("LuaVector of ints")
	{
		LuaVector<int> v;
			WHEN("index 0 is accessed")
		{
			THEN("exception should be thrown")
			{
				REQUIRE_THROWS_WITH(v[0], "LuaVector does not have index 0.");
			}
		}
	}
}

SCENARIO("LuaVector correctly constructs elements in-place", tag)
{
	GIVEN("LuaVector of strings")
	{
		auto v = LuaVectorWithStrings(5);
		
		REQUIRE(v.size() == 5);

		REQUIRE(v[1] == "Element number 1");
		REQUIRE(v[5] == "Element number 5");

		WHEN("EmplaceBack is called")
		{
			std::string simulating_rvalue = "Test string";
			auto it = v.EmplaceBack(std::move(simulating_rvalue));

			THEN("object is sucessfully created in-place via move construction")
			{
				REQUIRE(simulating_rvalue.empty());
				REQUIRE(v.size() == 6);

				REQUIRE(v[6] == "Test string");
			}
		}
		WHEN("Emplace is called")
		{
			auto it = std::begin(v);
			it += 2;

			REQUIRE((*it) == "Element number 3");

			std::string simulating_rvalue = "Test string";
			it = v.Emplace(it, std::move(simulating_rvalue));

			THEN("object is sucessfully created in-place")
			{
				REQUIRE(simulating_rvalue.empty());
				REQUIRE(v.size() == 6);
				REQUIRE((*it) == "Test string");
				auto prev = it - 1;
				auto next = it + 1;
				REQUIRE((*prev) == "Element number 2");
				REQUIRE((*next) == "Element number 3");
			}
		}
	}
}

SCENARIO("LuaVector correctly inserts elements", tag)
{
	GIVEN("LuaVector of strings")
	{
		auto v = LuaVectorWithStrings(5);

		WHEN("Insert is called on an r-value")
		{
			string text = "Test string";
			auto it = std::begin(v) + 3;
			v.Insert(it, std::move(text));

			THEN("object is correctly inserted into index, r-value suffers move operation")
			{
				REQUIRE(text.empty());

				REQUIRE(v.size() == 6);

				REQUIRE(v[3] == "Element number 3");
				REQUIRE(v[4] == "Test string");
				REQUIRE(v[5] == "Element number 4");
			}
		}
		WHEN("Insert is called on an l-value")
		{
			string text = "Test string";
			auto it = std::begin(v) + 3;
			v.Insert(it, text);

			THEN("object is correctly inserted into index, l-value suffers only a move operation")
			{
				REQUIRE(!text.empty());

				REQUIRE(v.size() == 6);

				REQUIRE(v[3] == "Element number 3");
				REQUIRE(v[4] == "Test string");
				REQUIRE(v[5] == "Element number 4");
			}
		}
		WHEN("push_back is called")
		{
			REQUIRE(v.back() == "Element number 5");

			v.PushBack("Last element");

			THEN("object is sucessfully inserted at the end of the LuaVector")
			{
				REQUIRE(v.size() == 6);
				REQUIRE(v.back() == "Last element");
			}
		}

	}
}

SCENARIO("IteratorFromIndex works correctly", tag)
{
	GIVEN("a default constructed LuaVector")
	{
		LuaVector<int> lvec;

		REQUIRE_THROWS_AS(lvec.IteratorFromIndex(-1), LuaVectorError); // can't exactly test for the text here because std::size_t underflow may differ depending on the environment
		REQUIRE_THROWS_WITH(lvec.IteratorFromIndex(0), "LuaVector does not have index 0.");
		REQUIRE_NOTHROW(lvec.IteratorFromIndex(1) == lvec.end());
		REQUIRE_THROWS_WITH(lvec.IteratorFromIndex(2), "LuaVector does not have index 2.");

		WHEN("elements are inserted into LuaVector")
		{
			lvec.PushBack(6);
			lvec.PushBack(5);
			lvec.PushBack(3);
			lvec.PushBack(1);
			lvec.PushBack(-6);

			THEN("IteratorFromIndex returns valid iterators")
			{
				auto it1 = lvec.IteratorFromIndex(1);
				REQUIRE(it1 != lvec.end());
				REQUIRE(it1 == lvec.begin());
				REQUIRE(*it1 == 6);

				auto it2 = lvec.IteratorFromIndex(2);
				REQUIRE(*it2 == 5);

				auto it3 = lvec.IteratorFromIndex(3);
				REQUIRE(*it3 == 3);

				auto it4 = lvec.IteratorFromIndex(4);
				REQUIRE(*it4 == 1);

				auto it5 = lvec.IteratorFromIndex(5);
				REQUIRE(*it5 == -6);

				REQUIRE_NOTHROW(lvec.IteratorFromIndex(6) == lvec.end());
				REQUIRE_THROWS_WITH(lvec.IteratorFromIndex(7), "LuaVector does not have index 7.");

			}
		}
	}
}

SCENARIO("auto_resize controls if LuaVector will resize when accessing non-existent index", tag)
{
	GIVEN("a LuaVector constructed with true as an argument")
	{
		LuaVector<int> v(true);
			WHEN("an out-of-range index (not zero) is accessed")
		{
			v[2];
			THEN("LuaVector resizes as necessary")
			{
				REQUIRE(v.size() == 2);
				REQUIRE(v[2] == 0);
			}
			AND_THEN("if auto_resize is turned off, the same kind of operation will throw")
			{
				v.SetAutoResize(false);
				REQUIRE_NOTHROW(v[2]);
				REQUIRE_THROWS_WITH(v[3], "LuaVector does not have index 3.");
			}
		}
		WHEN("a const reference is made")
		{
			v[4] = 4;
			const LuaVector<int>& v2 = v;
			THEN("accessing out-of-range indexes will throw")
			{
				REQUIRE_THROWS_WITH(v2[5], "LuaVector does not have index 5.");
			}
			AND_THEN("acessing indexes within the valid range will work")
			{
				REQUIRE_NOTHROW(v[4]);
			}
		}
	}
}

SCENARIO("LuaVector correctly erases elements", tag)
{
	GIVEN("a LuaVector of strings")
	{
		auto v = LuaVectorWithStrings(5);

		WHEN("erase is called")
		{
			auto pos = std::begin(v) + 2;
			REQUIRE(*pos == "Element number 3");

			auto it = v.Erase(pos);
			auto prev = it - 1;
			auto next = it + 1;
			THEN("the element is correctly erased")
			{
				REQUIRE(v.size() == 4);
				REQUIRE(*prev == "Element number 2");
				REQUIRE(*it == "Element number 4");
				REQUIRE(*next == "Element number 5");
			}
		}
	}
	
}

SCENARIO("LuaVector can correctly replace all its values", tag)
{
	GIVEN("a LuaVector of strings")
	{
		auto v = LuaVectorWithStrings(10);

		WHEN("ChangeAllValues is called")
		{
			v.ChangeAllValues("Nothing");
			THEN("all values must be changed")
			{
				bool correct = true;
				for (auto& string : v)
				{
					if (string != "Nothing")
					{
						correct = false;
						break;
					}
				}
				REQUIRE(correct);
			}
		}
	}
}

SCENARIO("a LuaVector can be created from a single element of the type it holds", tag)
{
	GIVEN("an initialized object")
	{
		string test = "Testing creation of LuaVector.";
		float test2 = 55.f;
		WHEN("constructor of a LuaVector holding the object's type is called with the object as argument")
		{
			LuaVector<string> v1 = test;
			LuaVector<float> v2 = test2;
			
			THEN("resulting LuaVector has size 1 and its first element equals the object passed as argument")
			{
				REQUIRE(v1.size() == 1);
				REQUIRE(v1[1] == "Testing creation of LuaVector.");

				REQUIRE(v2.size() == 1);
				REQUIRE(v2[1] == 55.f);
			}
		}	
	}
}

SCENARIO("a LuaVector can be created from a std::vector holding the same type", tag)
{
	GIVEN("an std::vector holding LuaVector's type")
	{
		vector<string> vec1{ "Testing LuaVector", "Another test string", "And this is the last one" };
		vector<float> vec2{ 55, 25, 68.5, 100 };

		WHEN("an std::vector is passed to LuaVector's constructor")
		{
			LuaVector<string> lvec1 = vec1;
			LuaVector<float> lvec2 = vec2;

			THEN("the resulting LuaVector is valid and its elements are equivalent to the std::vector's own")
			{
				REQUIRE(CompareLuaVectorToVector(lvec1, vec1));
				REQUIRE(CompareLuaVectorToVector(lvec2, vec2));

				lvec1.PushBack("Offsetting the balance");

				REQUIRE_FALSE(CompareLuaVectorToVector(lvec1, vec1));

				lvec1.PopBack();

				REQUIRE(CompareLuaVectorToVector(lvec1, vec1));

				vec2[1] = 0;

				REQUIRE_FALSE(CompareLuaVectorToVector(lvec2, vec2));
			}
		}
	}
}

SCENARIO("two LuaVectors holding the same type can be compared", tag)
{
	GIVEN("two LuaVectors holding strings")
	{
		LuaVector<string> v1(true);
		LuaVector<string> v2(true);

		WHEN("operator== is called")
		{
			THEN("comparisons are accurate")
			{
				REQUIRE(v1 == v2);
				v1[1] = "Testing LuaVector";
				REQUIRE_FALSE(v1 == v2);
				v2[1] = "Testing LuaVector";
				REQUIRE(v1 == v2);
				v1[5] = "Another test string";
				REQUIRE_FALSE(v1 == v2);
				v2[3] = "Yet another test string";
				REQUIRE_FALSE(v1 == v2);
				v2[5] = "Another test string";
				v1[3] = "Yet another test string";
				REQUIRE(v1 == v2);

				v1.Clear();
				v2.Clear();
				REQUIRE(v1.Empty());
				REQUIRE(v2.Empty());

				for (int i = 1; i <= 5; i++)
				{
					v1[i] = "Test number " + std::to_string(i);
					v2[i] = "Test number " + std::to_string(i);
				}
				REQUIRE(v1 == v2);
				v2.PopBack();
				REQUIRE_FALSE(v1 == v2);
				v1.PopBack();
				REQUIRE(v1 == v2);

				v1[15] = "A string, far from the others";

				REQUIRE_FALSE(v1 == v2);
			}
		}
	}
}