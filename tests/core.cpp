
// dependencies
#include <framework/system.cpp>
#include <framework/event.cpp>
#include <platform/time.cpp>
#include <framework/uid.cpp>

// testable unit
#include <framework/core.cpp>

// test framework
#include <templates/test.h>

START_TEST

TEST_CASE("Core Init and Update", "[core1]") {
	tram::Core::Init();
	
	auto first_tick = tram::GetTick();
	
	ASSERT(first_tick == 0)
	
	tram::Core::Update();
	
	ASSERT(tram::GetTick() == first_tick + 1)
}

TEST_CASE("Generate ID", "[core2]") {
	auto id1 = tram::GenerateID();
	auto id2 = tram::GenerateID();
	auto id3 = tram::GenerateID();
	
	ASSERT(id1 != id2)
	ASSERT(id2 != id3)
	ASSERT(id1 != id3)
	
}

END_TEST