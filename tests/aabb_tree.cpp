
// dependencies
#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <algorithm>
#include <cassert>
#include <chrono>

// testable unit
#include <templates/aabb.h>

// test framework
#include <templates/test.h>

using namespace tram;

struct Sphere {
	vec3 center;
	float radius;
	uint32_t id;
};

std::pair<vec3, vec3> GetSphereAABB(const Sphere& sphere) {
	vec3 min = sphere.center - vec3(sphere.radius);
	vec3 max = sphere.center + vec3(sphere.radius);
	return {min, max};
}

float RaySphereIntersection(vec3 ray_pos, vec3 ray_dir, const Sphere& sphere) {
	vec3 oc = ray_pos - sphere.center;
	float a = glm::dot(ray_dir, ray_dir);
	float b = 2.0f * glm::dot(oc, ray_dir);
	float c = glm::dot(oc, oc) - sphere.radius * sphere.radius;
	float discriminant = b * b - 4.0f * a * c;
	
	if (discriminant < 0.0f) {
		return INFINITY;
	}
	
	float t1 = (-b - std::sqrt(discriminant)) / (2.0f * a);
	float t2 = (-b + std::sqrt(discriminant)) / (2.0f * a);
	
	if (t1 > 0.0f) return t1;
	if (t2 > 0.0f) return t2;
	
	return INFINITY;
}

std::pair<uint32_t, float> BruteForceFindClosest(vec3 pos, vec3 dir, 
												 const std::vector<Sphere>& spheres,
												 float distance_limit = INFINITY)
{
	float min_dist = INFINITY;
	uint32_t closest_id = -1;

	for (const auto& sphere : spheres) {
		float dist = RaySphereIntersection(pos, dir, sphere);
		if (dist < min_dist && dist < distance_limit) {
			min_dist = dist;
			closest_id = sphere.id;
		}
	}
	
	return {closest_id, min_dist};
}

std::vector<Sphere> GenerateSpheres(size_t count,
									float r_min, float r_max,
									vec3 pos_min, vec3 pos_max,
									uint32_t seed = 420)
{
	std::mt19937 rng(seed);
	std::uniform_real_distribution<float> radius_dist(r_min, r_max);
	std::uniform_real_distribution<float> x_dist(pos_min.x, pos_max.x);
	std::uniform_real_distribution<float> y_dist(pos_min.y, pos_max.y);
	std::uniform_real_distribution<float> z_dist(pos_min.z, pos_max.z);

	std::vector<Sphere> spheres;
	for (size_t i = 0; i < count; ++i) {
		spheres.push_back({
			.center = vec3(x_dist(rng), y_dist(rng), z_dist(rng)),
			.radius = radius_dist(rng),
			.id = static_cast<uint32_t>(i)
		});
	}

	return spheres;
}

struct Ray {
	vec3 pos;
	vec3 dir;
};

std::vector<Ray> GenerateRays(size_t count,
							  vec3 pos_min, vec3 pos_max,
							  uint32_t seed = 69)
{
	std::mt19937 rng(seed);
	std::uniform_real_distribution<float> x_dist(pos_min.x, pos_max.x);
	std::uniform_real_distribution<float> y_dist(pos_min.y, pos_max.y);
	std::uniform_real_distribution<float> z_dist(pos_min.z, pos_max.z);
	std::uniform_real_distribution<float> dir_dist(-1.0f, 1.0f);
	
	std::vector<Ray> rays;
	for (size_t i = 0; i < count; ++i) {
		rays.push_back({
			.pos = {x_dist(rng), y_dist(rng), z_dist(rng)},
			.dir = glm::normalize(vec3{dir_dist(rng), dir_dist(rng), dir_dist(rng)})
		});
	}
	
	return rays;
}



START_TEST

TEST_CASE("AABB Tree Basic Raycasts", "[aabb1]") {
	AABBTree tree;
	
	// create a small set of spheres >_<
	std::vector<Sphere> spheres = GenerateSpheres(
		50,
		0.5f, 5.0f,
		{-50.0f, -50.0f, -50.0f}, {50.0f, 50.0f, 50.0f}
	);
	
	std::vector<Sphere> sphere_map(spheres.size());
	
	std::vector<AABBTree::Node*> nodes;
	for (const auto& sphere : spheres) {
		auto [min, max] = GetSphereAABB(sphere);
		auto* node = tree.InsertLeaf(sphere.id, min, max);
		nodes.push_back(node);
		sphere_map[sphere.id] = sphere;
	}
	
	std::vector<Ray> rays = GenerateRays(10000, {-60.0f, -60.0f, -60.0f}, {60.0f, 60.0f, 60.0f});
	
	for (const auto& ray : rays) {
		auto [brute_id, brute_dist] = BruteForceFindClosest(ray.pos, ray.dir, spheres);
		
		uint32_t tree_id = tree.FindIntersection(
			ray.pos, 
			ray.dir, 
			INFINITY,
			[&sphere_map](vec3 pos, vec3 dir, uint32_t index) -> float {
				return RaySphereIntersection(pos, dir, sphere_map[index]);
			}
		);
		
		ASSERT(tree_id == brute_id)
	}
	
	for (auto* node : nodes) {
		tree.RemoveLeaf(node);
	}
}

TEST_CASE("AABB Tree Insert/Delete", "[aabb2]") {
	AABBTree tree;

	std::mt19937 rng(421);
	std::vector<Sphere> all_spheres = GenerateSpheres(
		200,
		0.5f, 3.0f,
		{-100.0f, -100.0f, -100.0f}, {100.0f, 100.0f, 100.0f},
		421
	);
	
	std::vector<AABBTree::Node*> nodes(all_spheres.size(), nullptr);
	std::vector<bool> inserted(all_spheres.size(), false);
	
	for (int i = 0; i < 500; ++i) {
		std::uniform_int_distribution<size_t> idx_dist(0, all_spheres.size() - 1);
		size_t idx = idx_dist(rng);
		
		if (inserted[idx]) {
			if (nodes[idx] != nullptr) {
				tree.RemoveLeaf(nodes[idx]);
				nodes[idx] = nullptr;
				inserted[idx] = false;
			}
		} else {
			auto [min, max] = GetSphereAABB(all_spheres[idx]);
			nodes[idx] = tree.InsertLeaf(all_spheres[idx].id, min, max);
			inserted[idx] = true;
		}
	}
	
	size_t count = 0;
	std::vector<Sphere> current_spheres;
	for (size_t i = 0; i < all_spheres.size(); ++i) {
		if (inserted[i]) {
			count++;
			current_spheres.push_back(all_spheres[i]);
		}
	}
	
	std::vector<Ray> test_rays = GenerateRays(5000, {-110.0f, -110.0f, -110.0f}, {110.0f, 110.0f, 110.0f}, 516);
	
	for (const auto& ray : test_rays) {
		auto [brute_id, brute_dist] = BruteForceFindClosest(ray.pos, ray.dir, current_spheres);
		
		uint32_t tree_id = tree.FindIntersection(
			ray.pos,
			ray.dir,
			INFINITY,
			[&all_spheres](vec3 pos, vec3 dir, uint32_t index) -> float {
				return RaySphereIntersection(pos, dir, all_spheres[index]);
			}
		);
		
		ASSERT(tree_id == brute_id)
	}

	for (auto* node : nodes) {
		if (node != nullptr) {
			tree.RemoveLeaf(node);
		}
	}
}

TEST_CASE("AABB Tree Empty", "[aabb3]") {
	AABBTree tree;

	vec3 ray_pos = {0.0f, 0.0f, 0.0f};
	vec3 ray_dir = {1.0f, 0.0f, 0.0f};
	
	uint32_t result = tree.FindIntersection(
		ray_pos, ray_dir, INFINITY,
		[](vec3 pos, vec3 dir, uint32_t index) -> float {
			return INFINITY;
		}
	);
	
	ASSERT(result == -1)
}

TEST_CASE("AABB Tree Single", "[aabb4]") {
	AABBTree tree;

	std::vector<AABBTree::Node*> nodes;
	Sphere sphere = {
		.center = {10.0f, 0.0f, 0.0f},
		.radius = 2.0f,
		.id = 420
	};

	auto [min, max] = GetSphereAABB(sphere);
	auto* node = tree.InsertLeaf(sphere.id, min, max);
	nodes.push_back(node);
	
	vec3 ray_pos = {0.0f, 0.0f, 0.0f};
	vec3 ray_dir = {1.0f, 0.0f, 0.0f};
	
	uint32_t result = tree.FindIntersection(
		ray_pos, ray_dir, INFINITY,
		[&sphere](vec3 pos, vec3 dir, uint32_t index) -> float {
			return RaySphereIntersection(pos, dir, sphere);
		}
	);
	
	ASSERT (result == 420)
}


TEST_CASE("AABB Tree Overlapping", "[aabb5]") {
	AABBTree tree;

	std::vector<AABBTree::Node*> nodes;
	Sphere sphere = {
		.center = {10.0f, 0.0f, 0.0f},
		.radius = 1.0f,
		.id = 69
	};

	auto [min, max] = GetSphereAABB(sphere);
	nodes.push_back(tree.InsertLeaf(sphere.id, min, max));
	
	std::vector<Sphere> both_spheres = {
		{{10.0f, 0.0f, 0.0f}, 2.0f, 42},
		{{10.0f, 0.0f, 0.0f}, 1.0f, 43}
	};
	
	vec3 ray_pos = {0.0f, 0.0f, 0.0f};
	vec3 ray_dir = {1.0f, 0.0f, 0.0f};
	
	auto [brute_id, brute_dist] = BruteForceFindClosest(ray_pos, ray_dir, both_spheres);
	
	uint32_t tree_id = tree.FindIntersection(
		ray_pos, ray_dir, INFINITY,
		[&both_spheres](vec3 pos, vec3 dir, uint32_t index) -> float {
			return RaySphereIntersection(pos, dir, both_spheres[index == 42 ? 0 : 1]);
		}
	);
	
	ASSERT (tree_id == brute_id)

	for (auto* node : nodes) {
		tree.RemoveLeaf(node);
	}
}

/*
	Current Results @ 7500 spheres & 25 000 raycasts

	Insertion:		16ms
	Raycasts:		5255ms
	Teardown:		5ms
	Total time:		5276ms
*/

TEST_CASE("AABB Tree Timed Test", "[aabb6]") {
	AABBTree tree;
	
	std::vector<Sphere> spheres = GenerateSpheres(
		7500,
		0.5f, 5.0f,
		{-500.0f, -500.0f, -500.0f}, {500.0f, 500.0f, 500.0f});
		
std::vector<Ray> rays = GenerateRays(25000, {-600.0f, -600.0f, -600.0f}, {600.0f, 600.0f, 600.0f});
	
	auto insertion_time = std::chrono::high_resolution_clock::now();
	
	std::vector<AABBTree::Node*> nodes;
	for (const auto& sphere : spheres) {
		auto [min, max] = GetSphereAABB(sphere);
		nodes.push_back(tree.InsertLeaf(sphere.id, min, max));
	}

	auto raycasting_time = std::chrono::high_resolution_clock::now();
	
	for (const auto& ray : rays) {
		uint32_t tree_id = tree.FindIntersection(
			ray.pos, ray.dir, INFINITY,
			[&spheres](vec3 pos, vec3 dir, uint32_t index) -> float {
				return RaySphereIntersection(pos, dir, spheres[index]);
			}
		);
		
		// just so this doesn't get compiled out
		ASSERT(tree_id != -2)
	}
	
	auto teardown_time = std::chrono::high_resolution_clock::now();
	
	for (auto* node : nodes) {
		tree.RemoveLeaf(node);
	}
	
	auto end_time = std::chrono::high_resolution_clock::now();
	
	auto insertion = std::chrono::duration_cast<std::chrono::milliseconds>(raycasting_time - insertion_time);
	auto raycasting = std::chrono::duration_cast<std::chrono::milliseconds>(teardown_time - raycasting_time);
	auto teardown = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - teardown_time);
	auto total = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - insertion_time);
	
	std::cout << "Insertion:\t" << insertion << std::endl;
	std::cout << "Raycasts:\t" << raycasting << std::endl;
	std::cout << "Teardown:\t" << teardown << std::endl;
	std::cout << "Total time:\t" << total << std::endl;
	
}

TEST_CASE("AABB Tree Shape Cast", "[aabb7]") {
	AABBTree tree;

	std::vector<Sphere> spheres = {
		{{0.0f, 0.0f, 0.0f}, 1.0f, 0},
		{{10.0f, 0.0f, 0.0f}, 1.0f, 1},
		{{0.0f, 10.0f, 0.0f}, 1.0f, 2},
		{{10.0f, 10.0f, 0.0f}, 1.0f, 3},
	};
	
	std::vector<AABBTree::Node*> nodes;
	for (const auto& sphere : spheres) {
		auto [min, max] = GetSphereAABB(sphere);
		nodes.push_back(tree.InsertLeaf(sphere.id, min, max));
	}
	
	vec3 min = {-2.0f, -2.0f, -2.0f};
	vec3 max = {12.0f, 2.0f, 2.0f};
	
	std::vector<uint32_t> found;
	tree.FindAABBIntersection(min, max, [&found](uint32_t id) {
		found.push_back(id);
	});
	
	std::sort(found.begin(), found.end());
	
	ASSERT(found.size() == 2 && found[0] == 0 && found[1] == 1)
	
	for (auto* node : nodes) {
		tree.RemoveLeaf(node);
	}
}

TEST_CASE("AABB Tree Degenerate Tree", "[aabb8]") {
	AABBTree tree;

	const int GRID_SIZE = 32;
	const float SPHERE_RADIUS = 1.0f;
	const float SPACING = 3.0f;
	
	std::vector<AABBTree::Node*> nodes;
	
	uint32_t sphere_id = 0;
	
	for (int y = 0; y < GRID_SIZE; ++y) {
		if (y % 2 == 0) {
			for (int x = 0; x < GRID_SIZE; ++x) {
				vec3 center = {x * SPACING, y * SPACING, 0.0f};
				vec3 min = center - vec3(SPHERE_RADIUS);
				vec3 max = center + vec3(SPHERE_RADIUS);
				
				nodes.push_back(tree.InsertLeaf(sphere_id++, min, max));
			}
		} else {
			for (int x = GRID_SIZE - 1; x >= 0; --x) {
				vec3 center = {x * SPACING, y * SPACING, 0.0f};
				vec3 min = center - vec3(SPHERE_RADIUS);
				vec3 max = center + vec3(SPHERE_RADIUS);
				
				nodes.push_back(tree.InsertLeaf(sphere_id++, min, max));
			}
		}

	}
	
	ASSERT(true)
}

END_TEST

