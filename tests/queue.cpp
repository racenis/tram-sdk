
// testable unit
#include <templates/queue.h>

// test framework
#include <templates/test.h>

using namespace tram;

START_TEST

TEST_CASE("Basic Queue operations", "[queue1]") {
	struct TestData {
		int a;
		int b;
	};
	
	Queue<TestData> queue("test_queue", 20);
	
	ASSERT(queue.size() == 0);
	
	queue.push(42, 84);
	ASSERT(queue.size() == 1);
	
	ASSERT(queue.front().a == 42);
	ASSERT(queue.front().b == 84);
	ASSERT(queue.back().a == 42);
	ASSERT(queue.back().b == 84);
	
	queue.pop();
	ASSERT(queue.size() == 0);
}

TEST_CASE("Queue conforms to FIFO", "[queue2]") {
	struct TestData {
		int a;
		int b;
	};
	
	Queue<TestData> queue("fifo_queue", 30);
	
	for (int i = 0; i < 10; i++) {
		queue.push(i, i * 10);
	}
	
	ASSERT(queue.size() == 10);
	
	for (int i = 0; i < 10; i++) {
		ASSERT(queue.front().a == i);
		ASSERT(queue.front().b == i * 10);
		queue.pop();
	}
	
	ASSERT(queue.size() == 0);
}

TEST_CASE("Queue front and back works", "[queue3]") {
	struct TestData {
		int a;
		int b;
	};
	
	Queue<TestData> queue("fb_queue", 20);
	
	queue.push(10, 20);
	ASSERT(queue.front().a == 10);
	ASSERT(queue.back().a == 10);
	
	queue.push(30, 40);
	ASSERT(queue.front().a == 10);
	ASSERT(queue.back().a == 30);
	
	queue.push(50, 60);
	ASSERT(queue.front().a == 10);
	ASSERT(queue.back().a == 50);
	
	queue.pop();
	ASSERT(queue.front().a == 30);
	ASSERT(queue.back().a == 50);
}

TEST_CASE("Queue elements modified through front and back", "[queue4]") {
	struct TestData {
		int a;
		int b;
	};
	
	Queue<TestData> queue("mod_queue", 15);
	
	queue.push(1, 2);
	queue.push(3, 4);
	queue.push(5, 6);
	
	// honestly idk if this should be the expected thing... like... it shouldn't
	// be done.. like.. who modifies their queue elements??? NO ONE!!!!
	queue.front().a = 100;
	queue.front().b = 200;
	
	queue.back().a = 999;
	queue.back().b = 888;
	
	ASSERT(queue.front().a == 100);
	ASSERT(queue.front().b == 200);
	ASSERT(queue.back().a == 999);
	ASSERT(queue.back().b == 888);
}

TEST_CASE("Queue can be conditionally popped", "[queue5]") {
	struct TestData {
		int a;
		int b;
	};
	
	Queue<TestData> queue("try_queue", 20);
	TestData value;
	
	bool success = queue.try_pop(value);
	ASSERT(success == false);
	ASSERT(queue.size() == 0);
	
	queue.push(10, 20);
	queue.push(30, 40);
	queue.push(50, 60);
	ASSERT(queue.size() == 3);
	
	success = queue.try_pop(value);
	ASSERT(success == true);
	ASSERT(value.a == 10);
	ASSERT(value.b == 20);
	ASSERT(queue.size() == 2);
	
	success = queue.try_pop(value);
	ASSERT(success == true);
	ASSERT(value.a == 30);
	ASSERT(value.b == 40);
	ASSERT(queue.size() == 1);
	
	success = queue.try_pop(value);
	ASSERT(success == true);
	ASSERT(value.a == 50);
	ASSERT(value.b == 60);
	ASSERT(queue.size() == 0);
	
	success = queue.try_pop(value);
	ASSERT(success == false);
}

TEST_CASE("Queue stress test", "[queue6]") {
	struct TestData {
		int a;
		int b;
	};
	
	Queue<TestData> queue("stress_queue", 500);
	
	for (int i = 0; i < 200; i++) {
		queue.push(i, i * 3);
	}
	
	ASSERT(queue.size() == 200);
	
	for (int i = 0; i < 100; i++) {
		ASSERT(queue.front().a == i);
		ASSERT(queue.front().b == i * 3);
		queue.pop();
	}
	
	ASSERT(queue.size() == 100);
	
	for (int i = 200; i < 350; i++) {
		queue.push(i, i * 3);
	}
	
	ASSERT(queue.size() == 250);
	
	int expected = 100;
	while (queue.size() > 0) {
		ASSERT(queue.front().a == expected);
		ASSERT(queue.front().b == expected * 3);
		queue.pop();
		expected++;
	}
	
	ASSERT(expected == 350);
}

TEST_CASE("Queue pushing and popping", "[queue7]") {
	struct TestData {
		int a;
		int b;
	};
	
	Queue<TestData> queue("alt_queue", 50);

	for (int i = 0; i < 20; i++) {
		queue.push(i * 10, i * 100);
		queue.push(i * 10 + 1, i * 100 + 10);

		ASSERT(queue.size() == i > 0 ? i - 1 + 2 : 2);
		ASSERT(i > 1 ? queue.front().a > 0 : queue.front().a == 0);
		
		if (i > 0) {
			queue.pop();
		}

	}
}

TEST_CASE("Queue circular buffer", "[queue8]") {
	struct TestData {
		int a;
		int b;
	};
	
	Queue<TestData> queue("circ_queue", 100);
	
	for (int cycle = 0; cycle < 5; cycle++) {
		for (int i = 0; i < 50; i++) {
			queue.push(cycle * 100 + i, cycle * 1000 + i);
		}
		
		ASSERT(queue.size() == 50);
		
		for (int i = 0; i < 50; i++) {
			ASSERT(queue.front().a == cycle * 100 + i);
			ASSERT(queue.front().b == cycle * 1000 + i);
			queue.pop();
		}
		
		ASSERT(queue.size() == 0);
	}
}

TEST_CASE("Queue works with int", "[queue9]") {
	Queue<int> queue("int_queue", 30);
	
	for (int i = 0; i < 15; i++) {
		queue.push(i * 7);
	}
	
	ASSERT(queue.size() == 15);
	ASSERT(queue.front() == 0);
	ASSERT(queue.back() == 14 * 7);
	
	for (int i = 0; i < 15; i++) {
		ASSERT(queue.front() == i * 7);
		queue.pop();
	}
	
	ASSERT(queue.size() == 0);
}

TEST_CASE("Queue can find out its size", "[queue10]") {
	struct TestData {
		int a;
		int b;
	};
	
	Queue<TestData> queue("size_queue", 100);
	
	size_t expected_size = 0;
	ASSERT(queue.size() == expected_size);
	
	for (int i = 0; i < 50; i++) {
		queue.push(i, i);
		expected_size++;
		ASSERT(queue.size() == expected_size);
	}
	
	for (int i = 0; i < 30; i++) {
		queue.pop();
		expected_size--;
		ASSERT(queue.size() == expected_size);
	}
	
	ASSERT(queue.size() == 20);
}

TEST_CASE("Queue single element", "[queue11]") {
	struct TestData {
		int a;
		int b;
	};
	
	Queue<TestData> queue("single_queue", 1);
	
	queue.push(42, 84);
	ASSERT(queue.size() == 1);
	ASSERT(queue.front().a == 42);
	ASSERT(queue.back().a == 42);
	ASSERT(queue.front().a == queue.back().a);
	
	queue.pop();
	ASSERT(queue.size() == 0);
}

TEST_CASE("Queue try_pop edge cases", "[queue12]") {
	struct TestData {
		int a;
		int b;
	};
	
	Queue<TestData> queue("trans_queue", 10);
	TestData value;
	
	ASSERT(queue.try_pop(value) == false);
	
	queue.push(123, 456);
	
	ASSERT(queue.try_pop(value) == true);
	ASSERT(value.a == 123);
	ASSERT(value.b == 456);
	
	ASSERT(queue.try_pop(value) == false);
}

TEST_CASE("Queue front and back non-destructive", "[queue13]") {
	struct TestData {
		int a;
		int b;
	};
	
	Queue<TestData> queue("nondes_queue", 20);
	
	queue.push(10, 20);
	queue.push(30, 40);
	queue.push(50, 60);
	
	for (int i = 0; i < 5; i++) {
		ASSERT(queue.front().a == 10);
		ASSERT(queue.size() == 3);
	}
	
	for (int i = 0; i < 5; i++) {
		ASSERT(queue.back().a == 50);
		ASSERT(queue.size() == 3);
	}
}

TEST_CASE("Queue pushing and popping again", "[queue14]") {
	struct TestData {
		int a;
		int b;
	};
	
	Queue<TestData> queue("rapid_queue", 100);
	TestData value;
	
	for (int i = 0; i < 50; i++) {
		queue.push(i, i * 2);
		queue.push(i + 100, (i + 100) * 2);
		
		bool success = queue.try_pop(value);
		ASSERT(success == true);
		
		if (i == 0) {
			ASSERT(value.a == 0);
		} else if (i % 2 == 1) {
			ASSERT(value.a == (i - 1) / 2 + 100);
		} else {
			ASSERT(value.a == i / 2);
		}
	}
	
	ASSERT(queue.size() == 50);
	
	for (int i = 0; i < 50; i++) {
		ASSERT(queue.try_pop(value) == true);

		const int k = 25 + i / 2;

		if (i % 2 == 0) {
			ASSERT(value.a == k);
		} else {
			ASSERT(value.a == k + 100);
		}
	}
	
	ASSERT(queue.size() == 0);
}

TEST_CASE("Queue filling up", "[queue15]") {
	struct TestData {
		int a;
		int b;
	};
	
	Queue<TestData> queue("full_queue", 100);
	
	for (int i = 0; i < 100; i++) {
		queue.push(i, i * 5);
	}
	
	ASSERT(queue.size() == 100);
	ASSERT(queue.front().a == 0);
	ASSERT(queue.back().a == 99);
	
	for (int i = 0; i < 100; i++) {
		ASSERT(queue.front().a == i);
		queue.pop();
	}
	
	ASSERT(queue.size() == 0);
}

END_TEST
