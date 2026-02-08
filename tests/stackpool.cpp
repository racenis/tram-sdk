
// testable unit
#include <templates/stackpool.h>

// test framework
#include <templates/test.h>

using namespace tram;

START_TEST

TEST_CASE("Basic Stackpool operations", "[stackpool1]") {
	struct TestData {
		int a;
		int b;
	};
	
	StackPool<TestData> pool("test_pool", 100);
	
	ASSERT(pool.size() == 0);
	
	TestData* ptr1 = pool.allocate(1);
	ASSERT(ptr1 != nullptr);
	ASSERT(pool.size() == 1);
	
	ptr1->a = 42;
	ptr1->b = 84;
	ASSERT(ptr1->a == 42);
	ASSERT(ptr1->b == 84);

	TestData* ptr2 = pool.allocate(1);
	ASSERT(ptr2 != nullptr);
	ASSERT(pool.size() == 2);

	ASSERT(ptr2 == ptr1 + 1);
}

TEST_CASE("Multiple Stackpool allocations", "[stackpool2]") {
	struct TestData {
		int a;
		int b;
	};
	
	StackPool<TestData> pool("multi_pool", 200);
	
	TestData* data1 = pool.allocate(10);
	ASSERT(data1 != nullptr);
	ASSERT(pool.size() == 10);
	
	for (int i = 0; i < 10; i++) {
		data1[i].a = i * 5;
		data1[i].b = i * 7;
	}
	
	for (int i = 0; i < 10; i++) {
		ASSERT(data1[i].a == i * 5);
		ASSERT(data1[i].b == i * 7);
	}
	
	TestData* data2 = pool.allocate(5);
	ASSERT(pool.size() == 15);
}

TEST_CASE("Stackpool reset actually resets", "[stackpool3]") {
	struct TestData {
		int a;
		int b;
	};
	
	StackPool<TestData> pool("reset_pool", 100);
	
	TestData* data1 = pool.allocate(10);
	ASSERT(pool.size() == 10);
	
	for (int i = 0; i < 10; i++) {
		data1[i].a = i;
		data1[i].b = i * 2;
	}
	
	pool.reset();
	ASSERT(pool.size() == 0);
	
	TestData* data2 = pool.allocate(5);
	ASSERT(pool.size() == 5);
	
	ASSERT(data2 == data1);
	
	for (int i = 0; i < 5; i++) {
		data2[i].a = i + 100;
		data2[i].b = i + 200;
	}
	
	ASSERT(data2[0].a == 100);
	ASSERT(data2[0].b == 200);
}

TEST_CASE("Stackpool iteration", "[stackpool4]") {
	struct TestData {
		int a;
		int b;
	};
	
	StackPool<TestData> pool("iter_pool", 50);
	
	ASSERT(pool.begin() == pool.end());

	pool.allocate(5);
	TestData* items = pool.begin();
	
	for (int i = 0; i < 5; i++) {
		items[i].a = i * 10;
		items[i].b = i * 20;
	}
	
	int count = 0;
	for (TestData* it = pool.begin(); it != pool.end(); it++) {
		ASSERT(it->a == count * 10);
		ASSERT(it->b == count * 20);
		count++;
	}
	
	ASSERT(count == 5);
}

TEST_CASE("Stackpool range-based for loop", "[stackpool5]") {
	struct TestData {
		int a;
		int b;
	};
	
	StackPool<TestData> pool("range_pool", 30);
	
	TestData* items = pool.allocate(8);
	for (int i = 0; i < 8; i++) {
		items[i].a = i;
		items[i].b = i * i;
	}

	int idx = 0;
	for (auto& it : pool) {
		ASSERT(it.a == idx);
		ASSERT(it.b == idx * idx);
		idx++;
	}
	
	ASSERT(idx == 8);
}

TEST_CASE("Stackpool stress test", "[stackpool6]") {
	struct TestData {
		int a;
		int b;
	};
	
	StackPool<TestData> pool("stress_pool", 1000);
	
	TestData* pointers[50];
	int sizes[50];
	
	for (int i = 0; i < 50; i++) {
		sizes[i] = (i % 10) + 1;
		pointers[i] = pool.allocate(sizes[i]);
		
		for (int j = 0; j < sizes[i]; j++) {
			pointers[i][j].a = i * 100 + j;
			pointers[i][j].b = i * 1000 + j;
		}
	}
	
	for (int i = 0; i < 50; i++) {
		for (int j = 0; j < sizes[i]; j++) {
			ASSERT(pointers[i][j].a == i * 100 + j);
			ASSERT(pointers[i][j].b == i * 1000 + j);
		}
	}
	
	TestData* expected = pool.begin();
	for (int i = 0; i < 50; i++) {
		ASSERT(pointers[i] == expected);
		expected += sizes[i];
	}
	ASSERT(expected == pool.end());
}

TEST_CASE("Stackpool multiply resets", "[stackpool7]") {
	struct TestData {
		int a;
		int b;
	};
	
	StackPool<TestData> pool("cycle_pool", 100);
	
	for (int cycle = 0; cycle < 5; cycle++) {
		ASSERT(pool.size() == 0);
		
		TestData* data = pool.allocate(10);
		
		for (int i = 0; i < 10; i++) {
			data[i].a = cycle * 100 + i;
			data[i].b = cycle * 1000 + i;
		}
		
		ASSERT(pool.size() == 10);

		for (int i = 0; i < 10; i++) {
			ASSERT(data[i].a == cycle * 100 + i);
			ASSERT(data[i].b == cycle * 1000 + i);
		}
		
		pool.reset();
	}
}

TEST_CASE("Stackpool with ints", "[stackpool8]") {
	StackPool<int> pool("int_pool", 200);
	
	int* arr = pool.allocate(20);
	ASSERT(pool.size() == 20);
	
	for (int i = 0; i < 20; i++) {
		arr[i] = i * 3;
	}
	
	for (int i = 0; i < 20; i++) {
		ASSERT(arr[i] == i * 3);
	}
	
	int idx = 0;
	for (int* data = pool.begin(); data != pool.end(); data++) {
		ASSERT(*data == idx * 3);
		idx++;
	}
}

TEST_CASE("Stackpool size is reported correctly", "[stackpool9]") {
	struct TestData {
		int a;
		int b;
	};
	
	StackPool<TestData> pool("size_pool", 300);
	
	size_t expected_size = 0;
	ASSERT(pool.size() == expected_size);

	int allocations[] = {5, 10, 3, 20, 7, 15};
	
	for (int alloc_size : allocations) {
		pool.allocate(alloc_size);
		expected_size += alloc_size;
		ASSERT(pool.size() == expected_size);
	}

	pool.reset();
	ASSERT(pool.size() == 0);
}

TEST_CASE("Stackpool with zero length allocation", "[stackpool11]") {
	struct TestData {
		int a;
		int b;
	};
	
	StackPool<TestData> pool("zero_pool", 50);
	
	TestData* data = pool.allocate(0);

	ASSERT(data == nullptr);
	ASSERT(pool.size() == 0);
}

TEST_CASE("Stackpool lots of tiny allocations", "[stackpool12]") {
	struct TestData {
		int a;
		int b;
	};
	
	StackPool<TestData> pool("single_pool", 100);
	
	TestData* data[50];
	for (int i = 0; i < 50; i++) {
		data[i] = pool.allocate(1);
		data[i]->a = i;
		data[i]->b = i * 2;
	}
	
	ASSERT(pool.size() == 50);
	
	for (int i = 1; i < 50; i++) {
		ASSERT(data[i] == data[i-1] + 1);
	}
	
	for (int i = 0; i < 50; i++) {
		ASSERT(data[i]->a == i);
		ASSERT(data[i]->b == i * 2);
	}
}

TEST_CASE("Stackpool fill up", "[stackpool13]") {
	struct TestData {
		int a;
		int b;
	};
	
	StackPool<TestData> pool("full_pool", 100);
	
	TestData* data1 = pool.allocate(95);
	ASSERT(pool.size() == 95);
	
	for (int i = 0; i < 95; i++) {
		data1[i].a = i;
		data1[i].b = i * 10;
	}
	
	TestData* data2 = pool.allocate(5);
	ASSERT(pool.size() == 100);
	
	ASSERT(data2 == data1 + 95);
	
	for (int i = 0; i < 95; i++) {
		ASSERT(data1[i].a == i);
		ASSERT(data1[i].b == i * 10);
	}
}

TEST_CASE("Stackpool multiply fill up", "[stackpool14]") {
	struct TestData {
		int a;
		int b;
	};
	
	StackPool<TestData> pool("preserve_pool", 150);
	
	pool.allocate(150);
	ASSERT(pool.size() == 150);
	
	pool.reset();
	ASSERT(pool.size() == 0);

	TestData* data = pool.allocate(150);
	ASSERT(data != nullptr);
	ASSERT(pool.size() == 150);
}

TEST_CASE("Stackpool resets to beginning", "[stackpool15]") {
	struct TestData {
		int a;
		int b;
	};
	
	StackPool<TestData> pool("persist_pool", 50);
	
	TestData* data1 = pool.allocate(10);
	for (int i = 0; i < 10; i++) {
		data1[i].a = i + 100;
		data1[i].b = i + 200;
	}
	
	pool.reset();
	
	TestData* data2 = pool.allocate(10);
	
	// might be letting implementation details into slip into the test...
	// ahh, who cares? no one will look at what the tests do anyway :Dd
	ASSERT(data1 == data2);
}

END_TEST
