
// testable unit
#include <templates/hashmap.h>

// test framework
#include <templates/test.h>

using namespace tram;

START_TEST

TEST_CASE("Hashmap basic operations", "[hashmap1]") {
	struct TestData {
		int a;
		int b;
	};
	
	Hashmap<TestData> map("test_map", 10);
	
	ASSERT(!map.Exists(23));
	
	TestData data{420, 67};
	map.Insert(23, data);
	
	ASSERT(map.Exists(23));
	
	TestData retrieved = map.Find(23);
	ASSERT(retrieved.a == 420);
	ASSERT(retrieved.b == 67);
	
	map.Remove(23);
	
	ASSERT(!map.Exists(23));
}

TEST_CASE("Hashmap initializer list", "[hashmap2]") {
	struct TestData {
		int a;
		int b;
	};
	
	Hashmap<TestData> map("init_map", 10, {
		{1, TestData{10, 20}},
		{2, TestData{30, 40}},
		{3, TestData{50, 60}}
	});
	
	ASSERT(map.Exists(1));
	ASSERT(map.Exists(2));
	ASSERT(map.Exists(3));
	
	ASSERT(map.Find(1).a == 10 && map.Find(1).b == 20);
	ASSERT(map.Find(2).a == 30 && map.Find(2).b == 40);
	ASSERT(map.Find(3).a == 50 && map.Find(3).b == 60);
}

TEST_CASE("Hashmap collision handling", "[hashmap3]") {
	struct TestData {
		int a;
		int b;
	};
	
	size_t max_size = 50;
	Hashmap<TestData> map("collision_map", max_size);
	
	uint32_t base_key = 100;
	uint32_t stride = 2 * max_size;
	
	for (int i = 0; i < 10; i++) {
		uint32_t key = base_key + i * stride;
		map.Insert(key, TestData{i * 10, i * 100});
	}
	
	for (int i = 0; i < 10; i++) {
		uint32_t key = base_key + i * stride;
		ASSERT(map.Exists(key));
		TestData data = map.Find(key);
		ASSERT(data.a == i * 10);
		ASSERT(data.b == i * 100);
	}
	
	map.Remove(base_key + 3 * stride);
	map.Remove(base_key + 5 * stride);
	map.Remove(base_key + 7 * stride);
	
	for (int i = 0; i < 10; i++) {
		uint32_t key = base_key + i * stride;
		if (i == 3 || i == 5 || i == 7) {
			ASSERT(!map.Exists(key));
		} else {
			ASSERT(map.Exists(key));
			TestData data = map.Find(key);
			ASSERT(data.a == i * 10);
			ASSERT(data.b == i * 100);
		}
	}
}

TEST_CASE("Hashmap array operator", "[hashmap4]") {
	struct TestData {
		int a;
		int b;
	};
	
	Hashmap<TestData> map("bracket_map", 10);
	map.Insert(23, TestData{420, 421});
	
	ASSERT(map[23].a == 420);
	ASSERT(map[23].b == 421);
	
	map[23].a = 69;
	map[23].b = 67;

	ASSERT(map[23].a == 69);
	ASSERT(map[23].b == 67);
}

TEST_CASE("Hashmap overload performance", "[hashmap5]") {
	struct TestData {
		int a;
		int b;
	};
	
	const size_t num_entries = 25000;
	const size_t padding = 10;
	
	// this is like the absolute WORST WORST case, since we are intentionally
	// generating collisions
	
	{
		size_t max_size = num_entries + num_entries + num_entries + num_entries;
		Hashmap<TestData> map("tiny load", max_size, padding);
		
		TIME_START("Tiny load        [25%]");
		for (size_t i = 0; i < num_entries; i++) {
			map.Insert(i * max_size, TestData{(int)i, (int)i * 2});
		}
		
		for (size_t i = 0; i < num_entries; i++) {
			TestData data = map.Find(i * max_size);
			ASSERT(data.a == i);
			ASSERT(data.b == i * 2);
		}
		TIME_FINISH("Tiny load        [25%]");
	}
	
	{
		size_t max_size = num_entries + num_entries;
		Hashmap<TestData> map("medium load", max_size, padding);
		
		TIME_START("Medium load      [50%]");
		for (size_t i = 0; i < num_entries; i++) {
			map.Insert(i * max_size, TestData{(int)i, (int)i * 2});
		}
		
		for (size_t i = 0; i < num_entries; i++) {
			TestData data = map.Find(i * max_size);
			ASSERT(data.a == i);
			ASSERT(data.b == i * 2);
		}
		TIME_FINISH("Medium load      [50%]");
	}
	
	{
		size_t max_size = num_entries;
		Hashmap<TestData> map("full load", max_size, padding);
		
		TIME_START("Full load        [100%]");
		for (size_t i = 0; i < num_entries; i++) {
			map.Insert(i * max_size, TestData{(int)i, (int)i * 2});
		}
		
		for (size_t i = 0; i < num_entries; i++) {
			TestData data = map.Find(i * max_size);
			ASSERT(data.a == i);
			ASSERT(data.b == i * 2);
		}
		TIME_FINISH("Full load        [100%]");
	}
	
	{
		size_t max_size = num_entries / 2 + num_entries / 4;
		Hashmap<TestData> map("overload", max_size, padding);
		
		TIME_START("Overload         [150%]");
		for (size_t i = 0; i < num_entries; i++) {
			map.Insert(i * max_size, TestData{(int)i, (int)i * 2});
		}
		
		for (size_t i = 0; i < num_entries; i++) {
			TestData data = map.Find(i * max_size);
			ASSERT(data.a == i);
			ASSERT(data.b == i * 2);
		}
		TIME_FINISH("Overload         [150%]");
	}
	
	{
		size_t max_size = num_entries / 2 + 1;
		Hashmap<TestData> map("turbo overload", max_size, padding);
		
		TIME_START("Turbo overload   [200%]");
		for (size_t i = 0; i < num_entries; i++) {
			map.Insert(i * max_size, TestData{(int)i, (int)i * 2});
		}
		
		for (size_t i = 0; i < num_entries; i++) {
			TestData data = map.Find(i * max_size);
			ASSERT(data.a == i);
			ASSERT(data.b == i * 2);
		}
		TIME_FINISH("Turbo overload   [200%]");
	}
}

TEST_CASE("Hashmap non-existent access", "[hashmap6]") {
	struct TestData {
		int a;
		int b;
	};
	
	Hashmap<TestData> map("empty_map", 10);
	
	ASSERT(!map.Exists(0));
	ASSERT(!map.Exists(-420));
	ASSERT(!map.Exists(23));
}

TEST_CASE("Hashmap single entry map", "[hashmap7]") {
	struct TestData {
		int a;
		int b;
	};
	
	Hashmap<TestData> map("single_map", 1);
	
	map.Insert(23, TestData{67, 420});
	ASSERT(map.Exists(23));
	ASSERT(map.Find(23).a == 67);
	
	map.Remove(23);
	ASSERT(!map.Exists(23));
}

TEST_CASE("Hashmap iteration", "[hashmap8]") {
	struct TestData {
		int a;
		int b;
	};
	
	Hashmap<TestData> map("iteration map", 50);

	map.Insert(11, TestData{111, 222});
	map.Insert(22, TestData{333, 444});
	map.Insert(33, TestData{555, 666});
	map.Insert(44, TestData{777, 888});
	map.Insert(55, TestData{999, 1111});

	bool seen_11 = false;
	bool seen_22 = false;
	bool seen_33 = false;
	bool seen_44 = false;
	bool seen_55 = false;
	int count = 0;

	for (auto it = map.begin(); it != map.end(); ++it) {
		count++;

		if (it->a == 111 && it->b == 222) seen_11 = true;
		if (it->a == 333 && it->b == 444) seen_22 = true;
		if (it->a == 555 && it->b == 666) seen_33 = true;
		if (it->a == 777 && it->b == 888) seen_44 = true;
		if (it->a == 999 && it->b == 1111) seen_55 = true;
	}
	
	ASSERT(count == 5);
	ASSERT(seen_11);
	ASSERT(seen_22);
	ASSERT(seen_33);
	ASSERT(seen_44);
	ASSERT(seen_55);
}

TEST_CASE("Hashmap for loop iteration", "[hashmap9]") {
	struct TestData {
		int a;
		int b;
	};
	
	Hashmap<TestData> map("range map", 30);

	for (int i = 0; i < 10; i++) {
		map.Insert(i * 10, TestData{i, i * 2});
	}

	int count = 0;
	int sum_a = 0;
	int sum_b = 0;
	
	for (auto& item : map) {
		count++;
		sum_a += item.a;
		sum_b += item.b;
	}

	ASSERT(count == 10);
	ASSERT(sum_a == 0 + 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9);
	ASSERT(sum_b == 0 + 2 + 4 + 6 + 8 + 10 + 12 + 14 + 16 + 18);
}

TEST_CASE("Hashmap empty hashmap iteration", "[hashmap10]") {
	struct TestData {
		int a;
		int b;
	};
	
	Hashmap<TestData> map("empty map", 10);
	int count = 0;
	for (auto it = map.begin(); it != map.end(); ++it) {
		count++;
	}

	ASSERT(count == 0);

	count = 0;
	for (auto& item : map) {
		ASSERT(item.a == item.a)
		ASSERT(item.b == item.b)
		count++;
	}
	
	ASSERT(count == 0);
}

TEST_CASE("Hashmap empty begin is end", "[hashmap11]") {
	struct TestData {
		int a;
		int b;
	};
	
	Hashmap<TestData> map("empty map", 15);
	
	ASSERT(map.begin() == map.end());
}

TEST_CASE("Hashmap iteration over removed", "[hashmap12]") {
	struct TestData {
		int a;
		int b;
	};
	
	Hashmap<TestData> map("remove map", 40);
	
	for (int i = 0; i < 20; i++) {
		map.Insert(i, TestData{i * 5, i * 7});
	}
	
	map.Remove(5);
	map.Remove(10);
	map.Remove(15);
	
	int count = 0;
	bool seen_5 = false;
	bool seen_10 = false;
	bool seen_15 = false;
	
	for (auto& item : map) {
		count++;
		if (item.a == 25) seen_5 = true;
		if (item.a == 50) seen_10 = true;
		if (item.a == 75) seen_15 = true;
	}
	
	ASSERT(count == 17);

	ASSERT(!seen_5);
	ASSERT(!seen_10);
	ASSERT(!seen_15);
}

END_TEST
