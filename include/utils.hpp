#pragma once
// TODO: Eventually get external dependencies to maintain this code

// Combinatorial example
#include <vector>
#include <functional>
namespace typecheck::utils {
	std::vector<std::vector<int>> combinations(const int N, const int K, std::function<bool(const std::vector<int>&)>&& f);
	std::vector<std::vector<int>> combinations(const int N, const int K);
}
