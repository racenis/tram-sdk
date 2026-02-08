
// testable unit
#include <templates/stack.h>

// test framework
#include <templates/test.h>

using namespace tram;

START_TEST

TEST_CASE("Basic Stack operations", "[stack1]") {
	struct TestData {
		int a;
		int b;
	};
	
	Stack<TestData> stack("test_stack", 10);
	
	ASSERT(stack.size() == 0);
	
	TestData data1{42, 84};
	stack.push(data1);
	ASSERT(stack.size() == 1);
	
	ASSERT(stack.top().a == 42);
	ASSERT(stack.top().b == 84);
	ASSERT(stack.size() == 1);
	
	TestData popped = stack.pop();
	ASSERT(popped.a == 42);
	ASSERT(popped.b == 84);
	ASSERT(stack.size() == 0);
}

TEST_CASE("Stack conforms to LIFO", "[stack2]") {
	struct TestData {
		int a;
		int b;
	};
	
	Stack<TestData> stack("lifo_stack", 20);
	
	for (int i = 0; i < 5; i++) {
		stack.push(TestData{i, i * 10});
	}
	
	ASSERT(stack.size() == 5);
	
	for (int i = 4; i >= 0; i--) {
		ASSERT(stack.size() == static_cast<size_t>(i + 1));
		TestData popped = stack.pop();
		ASSERT(popped.a == i);
		ASSERT(popped.b == i * 10);
	}
	
	ASSERT(stack.size() == 0);
}

TEST_CASE("Stack can be peeked at", "[stack3]") {
	struct TestData {
		int a;
		int b;
	};
	
	Stack<TestData> stack("top_stack", 10);
	
	stack.push(TestData{100, 200});
	stack.push(TestData{300, 400});
	stack.push(TestData{500, 600});
	
	for (int i = 0; i < 10; i++) {
		ASSERT(stack.top().a == 500);
		ASSERT(stack.top().b == 600);
		ASSERT(stack.size() == 3);
	}
	
	stack.pop();
	ASSERT(stack.top().a == 300);
	ASSERT(stack.top().b == 400);
	ASSERT(stack.size() == 2);
}

TEST_CASE("Stack top element modification", "[stack4]") {
	struct TestData {
		int a;
		int b;
	};
	
	Stack<TestData> stack("modify_stack", 10);
	
	stack.push(TestData{10, 20});
	
	stack.top().a = 999;
	stack.top().b = 888;
	
	// honestly idk, maybe we don't need this?
	// I think that this method was meant for just peeking at data originally?
	ASSERT(stack.top().a == 999);
	ASSERT(stack.top().b == 888);
	
	TestData popped = stack.pop();
	ASSERT(popped.a == 999);
	ASSERT(popped.b == 888);
}

TEST_CASE("Stack reset", "[stack5]") {
	struct TestData {
		int a;
		int b;
	};
	
	Stack<TestData> stack("reset_stack", 50);
	
	for (int i = 0; i < 20; i++) {
		stack.push(TestData{i, i * 2});
	}
	
	ASSERT(stack.size() == 20);
	
	stack.reset();
	
	ASSERT(stack.size() == 0);
	
	stack.push(TestData{42, 84});
	ASSERT(stack.size() == 1);
	ASSERT(stack.top().a == 42);
}

TEST_CASE("Stack stress test", "[stack6]") {
	struct TestData {
		int a;
		int b;
	};
	
	Stack<TestData> stack("stress_stack", 500);
	
	for (int i = 0; i < 300; i++) {
		stack.push(TestData{i, i * 3});
	}
	
	ASSERT(stack.size() == 300);

	for (int i = 299; i >= 150; i--) {
		TestData popped = stack.pop();
		ASSERT(popped.a == i);
		ASSERT(popped.b == i * 3);
	}
	
	ASSERT(stack.size() == 150);
	
	for (int i = 300; i < 400; i++) {
		stack.push(TestData{i, i * 3});
	}
	
	ASSERT(stack.size() == 250);

	for (int i = 399; i >= 300; i--) {
		TestData popped = stack.pop();
		ASSERT(popped.a == i);
		ASSERT(popped.b == i * 3);
	}
	
	for (int i = 149; i >= 0; i--) {
		TestData popped = stack.pop();
		ASSERT(popped.a == i);
		ASSERT(popped.b == i * 3);
	}
	
	ASSERT(stack.size() == 0);
}

TEST_CASE("Stack works with ints", "[stack7]") {
	Stack<int> stack("int_stack", 20);

	for (int i = 0; i < 10; i++) {
		stack.push(i * 5);
	}
	
	ASSERT(stack.size() == 10);

	for (int i = 9; i >= 0; i--) {
		int popped = stack.pop();
		ASSERT(popped == i * 5);
	}
	
	ASSERT(stack.size() == 0);
}

TEST_CASE("Stack works with .. doubles?", "[stack8]") {
	Stack<double> stack("double_stack", 15);
	
	for (int i = 0; i < 8; i++) {
		stack.push(i * 1.5);
	}
	
	ASSERT(stack.size() == 8);
	
	ASSERT(stack.top() == 7 * 1.5);
	
	for (int i = 7; i >= 0; i--) {
		double popped = stack.pop();
		ASSERT(popped == i * 1.5);
	}
}

TEST_CASE("Stack resets multiple times", "[stack9]") {
	struct TestData {
		int a;
		int b;
	};
	
	Stack<TestData> stack("multi_reset_stack", 30);
	
	for (int cycle = 0; cycle < 5; cycle++) {
		// push items ;33
		for (int i = 0; i < 10; i++) {
			stack.push(TestData{i + cycle * 100, i + cycle * 1000});
		}
		
		ASSERT(stack.size() == 10);
		
		stack.reset();
		ASSERT(stack.size() == 0);
	}

	stack.push(TestData{999, 888});
	ASSERT(stack.size() == 1);
	ASSERT(stack.top().a == 999);
}

TEST_CASE("Stack handles single element", "[stack11]") {
	struct TestData {
		int a;
		int b;
	};
	
	Stack<TestData> stack("single_stack", 1);
	
	stack.push(TestData{42, 84});
	ASSERT(stack.size() == 1);
	ASSERT(stack.top().a == 42);
	
	TestData popped = stack.pop();
	ASSERT(popped.a == 42);
	ASSERT(stack.size() == 0);
}

TEST_CASE("Stack correctly does sizes", "[stack12]") {
	struct TestData {
		int a;
		int b;
	};
	
	Stack<TestData> stack("size_stack", 100);
	
	size_t expected_size = 0;
	ASSERT(stack.size() == expected_size);
	
	for (int i = 0; i < 50; i++) {
		stack.push(TestData{i, i});
		expected_size++;
		ASSERT(stack.size() == expected_size);
	}
	
	for (int i = 0; i < 30; i++) {
		stack.pop();
		expected_size--;
		ASSERT(stack.size() == expected_size);
	}

	stack.reset();
	expected_size = 0;
	ASSERT(stack.size() == expected_size);
}

TEST_CASE("Stack can be pushed and also popped", "[stack13]") {
	struct TestData {
		int a;
		int b;
	};
	
	Stack<TestData> stack("alt_stack", 50);
	
	for (int i = 0; i < 20; i++) {
		stack.push(TestData{i, i * 2});
		ASSERT(stack.size() == 1);
		
		TestData popped = stack.pop();
		ASSERT(popped.a == i);
		ASSERT(popped.b == i * 2);
		ASSERT(stack.size() == 0);
	}
}

TEST_CASE("Stack can be fully filled out", "[stack14]") {
	struct TestData {
		int a;
		int b;
	};
	
	Stack<TestData> stack("grow_stack", 5);
	
	for (int i = 0; i < 5; i++) {
		stack.push(TestData{i, i * 7});
	}
	
	ASSERT(stack.size() == 5);
	
	for (int i = 4; i >= 0; i--) {
		TestData popped = stack.pop();
		ASSERT(popped.a == i);
		ASSERT(popped.b == i * 7);
	}
	
	ASSERT(stack.size() == 0);
}

END_TEST
