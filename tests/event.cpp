
// testable unit
#include <framework/event.cpp>

// test framework
#include <templates/test.h>

using namespace tram;

// TODO: add some tests for getting events called on entities and components

START_TEST

TEST_CASE("Basic event", "[event1]") {
	static event_t event = Event::Register();
	
	ASSERT(event)
	
	static bool listener1_called = false;
	static bool listener2_called = false;
	static bool listener3_called = false;
	
	listener_t listener1 = Event::AddListener(event, [](Event&) { listener1_called = true; });
	listener_t listener2 = Event::AddListener(event, [](Event&) { listener2_called = true; });
	listener_t listener3 = Event::AddListener(event, [](Event&) { listener3_called = true; });
	
	listener_t listener4 = Event::AddListener(event, [](Event& e) {
		ASSERT(e.type == event)
		ASSERT(e.subtype == 444)
		ASSERT(e.poster_id == 111)
	});
	
	Event::Post({event, 444, 111});
	
	ASSERT(!listener1_called)
	ASSERT(!listener2_called)
	ASSERT(!listener3_called)
	
	Event::Dispatch();
	
	ASSERT(listener1_called)
	ASSERT(listener2_called)
	ASSERT(listener3_called)
	
	Event::Post({event, 444, 111});
	Event::Post({event, 444, 111});
	
	Event::Dispatch();
}

END_TEST