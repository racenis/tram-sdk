
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
        
        TIME_START("Tiny load      [25%]");
        for (size_t i = 0; i < num_entries; i++) {
            map.Insert(i * max_size, TestData{(int)i, (int)i * 2});
        }
        
        for (size_t i = 0; i < num_entries; i++) {
            TestData data = map.Find(i * max_size);
            ASSERT(data.a == i);
            ASSERT(data.b == i * 2);
        }
        TIME_FINISH("Tiny load      [25%]");
    }
	
	{
        size_t max_size = num_entries + num_entries;
        Hashmap<TestData> map("medium load", max_size, padding);
        
        TIME_START("Medium load    [50%]");
        for (size_t i = 0; i < num_entries; i++) {
            map.Insert(i * max_size, TestData{(int)i, (int)i * 2});
        }
        
        for (size_t i = 0; i < num_entries; i++) {
            TestData data = map.Find(i * max_size);
            ASSERT(data.a == i);
            ASSERT(data.b == i * 2);
        }
        TIME_FINISH("Medium load    [50%]");
    }
	
    {
        size_t max_size = num_entries;
        Hashmap<TestData> map("full load", max_size, padding);
        
        TIME_START("Full load      [100%]");
        for (size_t i = 0; i < num_entries; i++) {
            map.Insert(i * max_size, TestData{(int)i, (int)i * 2});
        }
        
        for (size_t i = 0; i < num_entries; i++) {
            TestData data = map.Find(i * max_size);
            ASSERT(data.a == i);
            ASSERT(data.b == i * 2);
        }
        TIME_FINISH("Full load      [100%]");
    }
    
    {
        size_t max_size = num_entries / 2 + num_entries / 4;
        Hashmap<TestData> map("overload", max_size, padding);
        
        TIME_START("Overload       [150%]");
        for (size_t i = 0; i < num_entries; i++) {
            map.Insert(i * max_size, TestData{(int)i, (int)i * 2});
        }
        
        for (size_t i = 0; i < num_entries; i++) {
            TestData data = map.Find(i * max_size);
            ASSERT(data.a == i);
            ASSERT(data.b == i * 2);
        }
        TIME_FINISH("Overload       [150%]");
    }
    
    {
        size_t max_size = num_entries / 2 + 1;
        Hashmap<TestData> map("turbo overload", max_size, padding);
        
        TIME_START("Turbo overload [200%]");
        for (size_t i = 0; i < num_entries; i++) {
            map.Insert(i * max_size, TestData{(int)i, (int)i * 2});
        }
        
        for (size_t i = 0; i < num_entries; i++) {
            TestData data = map.Find(i * max_size);
            ASSERT(data.a == i);
            ASSERT(data.b == i * 2);
        }
        TIME_FINISH("Turbo overload [200%]");
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

END_TEST
