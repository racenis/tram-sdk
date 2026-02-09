
// dependencies
#include <framework/system.cpp>
#include <framework/event.cpp>
#include <framework/logging.cpp>
#include <framework/uid.cpp>
#include <platform/other.cpp>

double TIME_MOCK_VALUE = 0.0;

namespace tram::Platform::Window {
	double GetTime() {
		return TIME_MOCK_VALUE;
	}
}

// testable unit
#include <framework/core.cpp>

// test framework
#include <templates/test.h>

START_TEST

TEST_CASE("Core Init and Update", "[core1]") {
	tram::Core::Init();
	tram::Core::Update();
	
	auto first_tick = tram::GetTick();
	
	ASSERT(first_tick == 0)
	
	TIME_MOCK_VALUE += 1.0/60.0 + 0.01;

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