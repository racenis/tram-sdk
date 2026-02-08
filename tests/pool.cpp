
// testable unit
#include <templates/pool.h>

// test framework
#include <templates/test.h>

using namespace tram;

START_TEST

TEST_CASE("Basic Pool operations", "[pool1]") {
	struct TestData {
		int a;
		int padding_1;
		int padding_2;
		int b;
	};
	
	Pool<TestData> pool("test_pool", 10);
	
	ASSERT(pool.size() == 0);
	
	TestData* data1 = pool.AddNew(420, 0, 0, 69);
	ASSERT(data1 != nullptr);
	ASSERT(data1->a == 420);
	ASSERT(data1->b == 69);
	ASSERT(pool.size() == 1);
	
	TestData* data2 = pool.AddNew(67, 0, 0, -420);
	ASSERT(data2->a == 67);
	ASSERT(data2->b == -420);
	ASSERT(pool.size() == 2);
	
	pool.Remove(data1);
	ASSERT(pool.size() == 1);
	
	ASSERT(data2->a == 67);
	ASSERT(data2->b == -420);
	
	pool.Remove(data2);
	ASSERT(pool.size() == 0);
}

TEST_CASE("Pool initializer list", "[pool2]") {
	struct TestData {
		int a;
		int padding_1;
		int padding_2;
		int b;
	};
	
	Pool<TestData> pool("init_pool", 10, {
		{10, 0, 0, 20},
		{30, 0, 0, 40},
		{50, 0, 0, 60},
		{70, 0, 0, 80}
	});
	
	ASSERT(pool.size() == 4);

	int expected_a[] = {10, 30, 50, 70};
	int expected_b[] = {20, 40, 60, 80};
	int idx = 0;
	
	for (auto& item : pool) {
		ASSERT(item.a == expected_a[idx]);
		ASSERT(item.b == expected_b[idx]);
		idx++;
	}
	
	ASSERT(idx == 4);
}

TEST_CASE("Pool stress test", "[pool3]") {
	struct TestData {
		int a;
		int padding_1;
		int padding_2;
		int b;
	};
	
	Pool<TestData> pool("stress_pool", 250);
	TestData* data[200];
	
	for (int i = 0; i < 200; i++) {
		data[i] = pool.AddNew(i, 0, 0, i * 2);
		ASSERT(data[i]->a == i);
		ASSERT(data[i]->b == i * 2);
	}
	ASSERT(pool.size() == 200);
	
	for (int i = 0; i < 200; i += 2) {
		pool.Remove(data[i]);
		data[i] = nullptr;
	}
	ASSERT(pool.size() == 100);
	
	for (int i = 1; i < 200; i += 2) {
		ASSERT(data[i]->a == i);
		ASSERT(data[i]->b == i * 2);
	}
	
	for (int i = 0; i < 200; i += 2) {
		data[i] = pool.AddNew(i + 1000, 0, 0, (i + 1000) * 2);
		ASSERT(data[i]->a == i + 1000);
		ASSERT(data[i]->b == (i + 1000) * 2);
	}
	ASSERT(pool.size() == 200);
	
	for (int i = 0; i < 200; i += 2) {
		ASSERT(data[i]->a == i + 1000);
		ASSERT(data[i]->b == (i + 1000) * 2);
	}
	for (int i = 1; i < 200; i += 2) {
		ASSERT(data[i]->a == i);
		ASSERT(data[i]->b == i * 2);
	}
}

TEST_CASE("Range-based for loop over a Pool", "[pool4]") {
	struct TestData {
		int padding_1;
		int a;
		int padding_2;
		int b;
	};
	
	Pool<TestData> pool("iter_pool", 20);
	
	for (int i = 0; i < 10; i++) {
		pool.AddNew(1, i * 10, 0, i * 100);
	}
	
	int count = 0;
	int sum_a = 0;
	int sum_b = 0;
	
	for (auto& item : pool) {
		sum_a += item.a;
		sum_b += item.b;
		count++;
	}
	
	ASSERT(count == 10);
	ASSERT(sum_a == 0 + 10 + 20 + 30 + 40 + 50 + 60 + 70 + 80 + 90); // = 450 >_
	ASSERT(sum_b == 0 + 100 + 200 + 300 + 400 + 500 + 600 + 700 + 800 + 900); // = 4500 ;33
}

TEST_CASE("Pool iterator modification", "[pool5]") {
	struct TestData {
		int padding_1;
		int a;
		int padding_2;
		int b;
	};
	
	Pool<TestData> pool("modify_pool", 10);
	
	for (int i = 0; i < 5; i++) {
		pool.AddNew(1, i, 0, i);
	}
	
	for (auto& item : pool) {
		item.a *= 2;
		item.b *= 3;
	}
	
	int idx = 0;
	for (auto& item : pool) {
		ASSERT(item.a == idx * 2);
		ASSERT(item.b == idx * 3);
		idx++;
	}
}

TEST_CASE("Pool with 64 leading zeroes", "[pool6]") {
	struct TestData {
		int a;
		int b;
		int c;
		int d;
	};
	
	Pool<TestData> pool("iter_pool", 20);
	
	for (int i = 0; i < 10; i++) {
		pool.AddNew(0, 0, 111, 222);
	}
	
	int count = 0;
	
	// the iterator will skip over all elements, since they begin with at least
	// 64 bits that are all zeroes (assuming that default 2 x default int size
	// is at least 64 bits in length)
	for (auto& item : pool) {
		count++;
	}
	
	ASSERT(count == 0);
}


TEST_CASE("Pool pointer-index conversion", "[pool7]") {
	struct TestData {
		int a;
		int padding_1;
		int padding_2;
		int b;
	};
	
	Pool<TestData> pool("index_pool", 50);
	TestData* pointers[20];
	
	for (int i = 0; i < 20; i++) {
		pointers[i] = pool.AddNew(i * 5, 0, 0, i * 7);
	}
	
	for (int i = 0; i < 20; i++) {
		size_t idx = pool.index(pointers[i]);
		TestData& ref = pool[idx];
		
		ASSERT(&ref == pointers[i]);
		ASSERT(ref.a == i * 5);
		ASSERT(ref.b == i * 7);
	}
}

TEST_CASE("Pool pointer validation", "[pool8]") {
	struct TestData {
		int a;
		int padding_1;
		int padding_2;
		int b;
	};
	
	Pool<TestData> pool("valid_pool", 30);
	
	TestData* ptr1 = pool.AddNew(3526, 223);
	ASSERT(pool.validate(ptr1) == true);
	
	pool.Remove(ptr1);
	ASSERT(pool.validate(ptr1) == false);
	
	TestData stack_data{314, 1515};
	TestData* random_ptr = &stack_data;
	ASSERT(pool.validate(random_ptr) == false);
	
	TestData* null_ptr = nullptr;
	ASSERT(pool.validate(null_ptr) == false);
	
	pool.AddNew(42, 0, 0, 84);
	pool.AddNew(11, 0, 0, 22);
	pool.AddNew(33, 0, 0, 44);
	
	TestData& ref = pool[0];
	ASSERT(pool.validate(&ref) == true);
	
	TestData& ref2 = pool[1];
	ASSERT(pool.validate(&ref2) == true);
	
	TestData& ref_bad = pool[999];
	ASSERT(pool.validate(&ref_bad) == false);
}

TEST_CASE("Pool size counting", "[pool9]") {
	struct TestData {
		int a;
		int padding_1;
		int padding_2;
		int b;
	};
	
	Pool<TestData> pool("size_pool", 100);
	
	ASSERT(pool.size() == 0);
	
	TestData* ptrs[50];
	for (int i = 0; i < 50; i++) {
		ptrs[i] = pool.AddNew(i, 0, 0, i);
		ASSERT(pool.size() == static_cast<size_t>(i + 1));
	}
	
	for (int i = 0; i < 50; i++) {
		pool.Remove(ptrs[i]);
		ASSERT(pool.size() == static_cast<size_t>(49 - i));
	}
	
	ASSERT(pool.size() == 0);
}

TEST_CASE("Empty pool iteration", "[pool11]") {
	struct TestData {
		int a;
		int padding_1;
		int padding_2;
		int b;
	};
	
	Pool<TestData> pool("empty_pool", 10);
	
	int count = 0;
	for (auto& item : pool) {
		ASSERT(item.a && !item.a);
		count++;
	}
	
	ASSERT(count == 0);
}

TEST_CASE("Pool with single element", "[pool12]") {
	struct TestData {
		int a;
		int padding_1;
		int padding_2;
		int b;
	};
	
	Pool<TestData> pool("single_pool", 1);
	
	TestData* ptr = pool.AddNew(767, 0, 0, 676);
	ASSERT(pool.size() == 1);
	ASSERT(pool.validate(ptr));
	ASSERT(pool.index(ptr) == 0);
	
	int count = 0;
	for (auto& item : pool) {
		ASSERT(item.a == 767);
		ASSERT(item.b == 676);
		count++;
	}
	ASSERT(count == 1);
	
	pool.Remove(ptr);
	ASSERT(pool.size() == 0);
	ASSERT(!pool.validate(ptr));
}

TEST_CASE("Pool iterators", "[pool13]") {
	struct TestData {
		int a;
		int padding_1;
		int padding_2;
		int b;
	};
	
	Pool<TestData> pool("iters_pool", 10);
	
	pool.AddNew(10, 0, 0, 20);
	pool.AddNew(30, 0, 0, 40);
	pool.AddNew(50, 0, 0, 60);
	
	auto it = pool.begin();
	auto end_it = pool.end();
	
	ASSERT(it != end_it);
	
	ASSERT((*it).a == 10);
	ASSERT(it->b == 20);
	
	++it;
	ASSERT(it->a == 30);
	ASSERT(it->b == 40);
	
	auto old_it = it++;
	ASSERT(old_it->a == 30);
	ASSERT(it->a == 50);
	
	++it;
	ASSERT(it == end_it);
}

TEST_CASE("Pool re-allocation", "[pool14]") {
	struct TestData {
		int a;
		int padding_1;
		int padding_2;
		int b;
	};
	
	Pool<TestData> pool("realloc_pool", 100);
	
	TestData* ptrs[10];
	for (int i = 0; i < 10; i++) {
		ptrs[i] = pool.AddNew(i, 0, 0, i * 10);
	}
	
	size_t idx_5 = pool.index(ptrs[5]);
	
	pool.Remove(ptrs[5]);
	ASSERT(pool.size() == 9);
	
	TestData* new_ptr = pool.AddNew(777, 0, 0, 369);
	ASSERT(pool.size() == 10);
	
	// technically not the best practice -- an implementation, even a correct
	// one doesn't need to return the same index as the just freed data
	size_t new_idx = pool.index(new_ptr);
	ASSERT(idx_5 == new_idx);
	
	ASSERT(new_ptr->a == 777);
	ASSERT(new_ptr->b == 369);
}

END_TEST
