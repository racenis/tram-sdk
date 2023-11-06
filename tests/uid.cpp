
// testable unit
#include <framework/uid.cpp>

// test framework
#include <templates/test.h>

using namespace tram;

START_TEST

TEST_CASE("UID various", "[uid1]") {
	ASSERT(UID("asdfghjkl"))
	
	ASSERT(UID() == UID())
	ASSERT(UID() == "none")
	ASSERT(UID() == UID("none"))
	
	ASSERT(!UID())
	
	ASSERT(UID("aaa") == UID("aaa"))
	ASSERT(UID("aaa") != UID("bbb"))
	
	ASSERT(strcmp(UID("aaa"), "aaa") == 0)
	ASSERT(strcmp(UID("aaa"), "bbb") != 0)
}

END_TEST