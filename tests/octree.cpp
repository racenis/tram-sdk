// uwu octree testing suite~ ;33 >_<
// testing the octree's nearest neighbor search against brute force!

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <array>
#include <cmath>
#include <set>
#include <glm/glm.hpp>

// testable unit
#include <templates/octree.h>

// test framework
#include <templates/test.h>

using namespace tram;

struct PointInfo {
	vec3 point;
	int data;
	Octree<int>::leaf_t leaf_id;
};

struct NearestResult {
	int data;
	float distance;
};

size_t BruteForceFindNearest(const std::vector<PointInfo>& points, vec3 query, NearestResult* result) {
	if (points.empty()) return 0;
	
	std::vector<std::pair<float, size_t>> distances;
	distances.reserve(points.size());
	
	for (size_t i = 0; i < points.size(); ++i) {
		float dist = glm::distance(points[i].point, query);
		distances.push_back({dist, i});
	}
	
	std::sort(distances.begin(), distances.end(),
		[](const auto& a, const auto& b) { return a.first < b.first; });
	
	size_t count = distances.size() > 4 ? 4 : distances.size();
	for (size_t i = 0; i < count; ++i) {
		result[i].data = points[distances[i].second].data;
		result[i].distance = distances[i].first;
	}
	
	return count;
}

bool CompareResults(const std::vector<PointInfo>& points, vec3 query, int* octree_results, size_t octree_count, NearestResult* brute_results, size_t brute_count) {
	if (octree_count != brute_count) {
		return false;
	}
	
	std::vector<float> octree_distances;
	for (size_t i = 0; i < octree_count; ++i) {
		for (const auto& p : points) {
			if (p.data == octree_results[i]) {
				octree_distances.push_back(glm::distance(p.point, query));
				break;
			}
		}
	}
	
	std::vector<float> brute_distances;
	for (size_t i = 0; i < brute_count; ++i) {
		brute_distances.push_back(brute_results[i].distance);
	}
	
	std::sort(octree_distances.begin(), octree_distances.end());
	std::sort(brute_distances.begin(), brute_distances.end());
	
	const float epsilon = 0.0001f;
	for (size_t i = 0; i < octree_count; ++i) {
		if (std::abs(octree_distances[i] - brute_distances[i]) > epsilon) {
			return false;
		}
	}
	
	return true;
}

std::vector<vec3> GenerateRandomPoints(size_t count, vec3 min_bounds, vec3 max_bounds, uint32_t seed = 420) {
	std::mt19937 rng(seed);
	std::uniform_real_distribution<float> x_dist(min_bounds.x, max_bounds.x);
	std::uniform_real_distribution<float> y_dist(min_bounds.y, max_bounds.y);
	std::uniform_real_distribution<float> z_dist(min_bounds.z, max_bounds.z);
	
	std::vector<vec3> points;
	points.reserve(count);
	
	for (size_t i = 0; i < count; ++i) {
		points.push_back({x_dist(rng), y_dist(rng), z_dist(rng)});
	}
	
	return points;
}

START_TEST

TEST_CASE("Basic Octree queries", "[octree1]") {
	Octree<int> octree({0.0f, 0.0f, 0.0f}, 1000.0f);
	
	std::vector<PointInfo> points;
	
	//auto random_points = GenerateRandomPoints(50, {-100.0f, -100.0f, -100.0f}, {100.0f, 100.0f, 100.0f});
	auto random_points = GenerateRandomPoints(10, {-100.0f, -100.0f, -100.0f}, {100.0f, 100.0f, 100.0f});
	
	for (size_t i = 0; i < random_points.size(); ++i) {
		//int data = i * 100;
		int data = i;
		Octree<int>::leaf_t leaf_id = octree.Insert(random_points[i], data);
		points.push_back({random_points[i], data, leaf_id});
	}
	
	std::vector<vec3> query_points = GenerateRandomPoints(30, {-120.0f, -120.0f, -120.0f}, {120.0f, 120.0f, 120.0f}, 421);
	//std::vector<vec3> query_points = GenerateRandomPoints(1, {-120.0f, -120.0f, -120.0f}, {120.0f, 120.0f, 120.0f}, 421);

	for (const auto& query : query_points) {
		int octree_results[4];
		NearestResult brute_results[4];
		
		size_t octree_count = octree.Find(octree_results, query);
		size_t brute_count = BruteForceFindNearest(points, query, brute_results);
		
		bool results_match = CompareResults(points, query, octree_results, octree_count, brute_results, brute_count);
		
		ASSERT(results_match)
	}
}

TEST_CASE("Random Octree queries", "[octree2]") {
	Octree<int> octree({0.0f, 0.0f, 0.0f}, 1000.0f);
	
	std::mt19937 rng(489);
	std::uniform_real_distribution<float> coord_dist(-200.0f, 200.0f);
	std::uniform_int_distribution<size_t> action_dist(0, 2);
	
	std::vector<PointInfo> active_points;
	
	for (int i = 0; i < 500; ++i) {
		size_t action = action_dist(rng);
		
		if (action == 0 || active_points.empty()) {
			
			vec3 point(coord_dist(rng), coord_dist(rng), coord_dist(rng));
			int data = i;
			Octree<int>::leaf_t leaf_id = octree.Insert(point, data);
			active_points.push_back({point, data, leaf_id});
			
		} else if (action == 1 && !active_points.empty()) {
			
			std::uniform_int_distribution<size_t> idx_dist(0, active_points.size() - 1);
			size_t idx = idx_dist(rng);
			
			octree.Remove(active_points[idx].leaf_id);
			active_points.erase(active_points.begin() + idx);
			
		} else if (action == 2 && !active_points.empty()) {
			
			vec3 query(coord_dist(rng), coord_dist(rng), coord_dist(rng));
			
			int octree_results[4];
			NearestResult brute_results[4];
			
			size_t octree_count = octree.Find(octree_results, query);
			size_t brute_count = BruteForceFindNearest(active_points, query, brute_results);
			
			bool results_match = CompareResults(active_points, query, octree_results, octree_count, brute_results, brute_count);
			
			ASSERT(results_match)
		}
	}
}

TEST_CASE("Octree edge cases", "[octree3]") {
	Octree<int> octree({0.0f, 0.0f, 0.0f}, 1000.0f);
	std::vector<PointInfo> points;
	
	// empty octree
	int results[4];
	size_t count = octree.Find(results, {0.0f, 0.0f, 0.0f});
	
	ASSERT(count == 0)
	
	// single point
	Octree<int>::leaf_t leaf_id = octree.Insert({4.0f, 2.0f, 0.0f}, 67);
	points.push_back({{4.0f, 2.0f, 0.0f}, 67, leaf_id});
	
	count = octree.Find(results, {4.0f, 2.0f, 0.0f});
	
	ASSERT(count == 1)
	ASSERT(results[0] == 67)
	
	// two points with same position
	Octree<int>::leaf_t leaf_id1 = octree.Insert({0.0f, 6.0f, 9.0f}, 200);
	Octree<int>::leaf_t leaf_id2 = octree.Insert({0.0f, 6.0f, 9.0f}, 201);
	
	points.push_back({{0.0f, 6.0f, 9.0f}, 200, leaf_id1});
	points.push_back({{0.0f, 6.0f, 9.0f}, 201, leaf_id2});

	count = octree.Find(results, {0.0f, 6.0f, 9.0f});
	
	bool found_data1 = false;
	bool found_data2 = false;
	for (size_t i = 0; i < count; ++i) {
		if (results[i] == 200) found_data1 = true;
		if (results[i] == 201) found_data2 = true;
	}
	
	ASSERT(found_data1)
	ASSERT(found_data2)
}

TEST_CASE("Octree quadrant edge cases", "[octree4]") {
	Octree<int> octree({0.0f, 0.0f, 0.0f}, 1000.0f);
	std::vector<PointInfo> points;
	
	std::vector<vec3> octant_points = {
		{-10, -10, -10},
		{10, -10, -10},
		{-10, 10, -10},
		{10, 10, -10},
		{-10, -10, 10},
		{10, -10, 10},
		{-10, 10, 10},
		{10, 10, 10},
	};
	
	for (size_t i = 0; i < octant_points.size(); ++i) {
		int data = static_cast<int>(i);
		Octree<int>::leaf_t leaf_id = octree.Insert(octant_points[i], data);
		points.push_back({octant_points[i], data, leaf_id});
	}
	
	Octree<int>::leaf_t origin_id = octree.Insert({0.0f, 0.0f, 0.0f}, 101);
	Octree<int>::leaf_t positive_id = octree.Insert({1.0f, 1.0f, 1.0f}, 102);
	
	points.push_back({{0.0f, 0.0f, 0.0f}, 101, origin_id});
	points.push_back({{1.0f, 1.0f, 1.0f}, 102, positive_id});
	
	int octree_results[4];
	NearestResult brute_results[4];
	
	std::vector<vec3> test_queries = {
		{0, 0, 0},
		{1, 1, 1},
		{5, 5, 5},
		{-5, -5, -5}
	};

	for (const auto& query : test_queries) {
		size_t octree_count = octree.Find(octree_results, query);
		size_t brute_count = BruteForceFindNearest(points, query, brute_results);
		
		bool results_match = CompareResults(points, query, octree_results, octree_count, brute_results, brute_count);
		
		ASSERT(results_match)
	}
}

TEST_CASE("Octree boundary cases", "[octree5]") {
	Octree<int> octree({0.0f, 0.0f, 0.0f}, 1000.0f);
	std::vector<PointInfo> points;
	
	std::vector<vec3> boundary_points = {
		{0, 10, 10},
		{10, 0, 10},
		{10, 10, 0},
		{0, 0, 10},
		{0, 10, 0},
		{10, 0, 0},
		{0, 0, 0},
	};
	
	for (size_t i = 0; i < boundary_points.size(); ++i) {
		int data = i * 10;
		Octree<int>::leaf_t leaf_id = octree.Insert(boundary_points[i], data);
		points.push_back({boundary_points[i], data, leaf_id});
	}
	
	std::vector<vec3> query_points = {
		{0.1f, 0.1f, 0.1f},
		{-0.1f, -0.1f, -0.1f},
		{5, 0, 0},
		{0, 5, 5},
	};
	
	for (const auto& query : query_points) {
		int octree_results[4];
		NearestResult brute_results[4];
		
		size_t octree_count = octree.Find(octree_results, query);
		size_t brute_count = BruteForceFindNearest(points, query, brute_results);
		
		bool results_match = CompareResults(points, query, octree_results, octree_count, brute_results, brute_count);
		
		ASSERT(results_match)
	}
}

TEST_CASE("Large Octree test", "[octree6]") {
	Octree<int> octree({0.0f, 0.0f, 0.0f}, 1000.0f);
	
	auto random_points1 = GenerateRandomPoints(1000, {-500.0f, -500.0f, -500.0f}, {500.0f, 500.0f, 500.0f});
	auto random_points2 = GenerateRandomPoints(1000, {-500.0f, -500.0f, -500.0f}, {500.0f, 500.0f, 500.0f});
	std::vector<PointInfo> points;
	
	auto query_points = GenerateRandomPoints(7500, {-600.0f, -600.0f, -600.0f}, {600.0f, 600.0f, 600.0f}, 5156);
	
	TIME_START("Initial insertion")
	for (size_t i = 0; i < random_points1.size(); ++i) {
		Octree<int>::leaf_t leaf_id = octree.Insert(random_points1[i], i);
		points.push_back({random_points1[i], (int)i, leaf_id});
	}
	TIME_FINISH("Initial insertion")
	
	std::vector<std::array<NearestResult, 4>> brute_results_set1;
	brute_results_set1.reserve(query_points.size());
	for (const auto& query : query_points) {
		std::array<NearestResult, 4> brute_results;
		BruteForceFindNearest(points, query, brute_results.data());
		brute_results_set1.push_back(brute_results);
	}
	
	TIME_START("Queries")
	for (size_t i = 0; i < query_points.size(); ++i) {
		int octree_results[4];
		
		size_t octree_count = octree.Find(octree_results, query_points[i]);
		
		bool results_match = CompareResults(points, query_points[i], octree_results, octree_count, 
											 brute_results_set1[i].data(), 4);
		
		ASSERT(results_match)
	}
	TIME_FINISH("Queries")
	
	TIME_START("Removing all points")
	for (auto& point : points) {
		octree.Remove(point.leaf_id);
	}
	TIME_FINISH("Removing all points")
	
	points.clear();
	
	TIME_START("Insertion of different points")
	for (size_t i = 0; i < random_points2.size(); ++i) {
		Octree<int>::leaf_t leaf_id = octree.Insert(random_points2[i], i);
		points.push_back({random_points2[i], (int)i, leaf_id});
	}
	TIME_FINISH("Insertion of different points")
	
	std::vector<std::array<NearestResult, 4>> brute_results_set2;
	brute_results_set2.reserve(query_points.size());
	for (const auto& query : query_points) {
		std::array<NearestResult, 4> brute_results;
		BruteForceFindNearest(points, query, brute_results.data());
		brute_results_set2.push_back(brute_results);
	}
	
	TIME_START("Queries, again")
	for (size_t i = 0; i < query_points.size(); ++i) {
		int octree_results[4];
		
		size_t octree_count = octree.Find(octree_results, query_points[i]);
		
		bool results_match = CompareResults(points, query_points[i], octree_results, octree_count, 
											 brute_results_set2[i].data(), 4);
		
		ASSERT(results_match)
	}
	TIME_FINISH("Queries, again")
	
	TIME_START("Teardown")
	for (auto& point : points) {
		octree.Remove(point.leaf_id);
	}
	TIME_FINISH("Teardown")
}

END_TEST
