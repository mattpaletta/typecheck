#include "typecheck/utils.hpp"
#include <string>
#include <algorithm>

std::vector<std::vector<int>> typecheck::utils::combinations(const int N, const int K) {
    return typecheck::utils::combinations(N, K, [](const std::vector<int>& a) { return true; });
}

// Taken from: https://stackoverflow.com/questions/12991758/creating-all-possible-k-combinations-of-n-items-in-c
std::vector<std::vector<int>> typecheck::utils::combinations(const int N, const int K, std::function<bool(const std::vector<int>&)>&& f) {
    std::string bitmask(K, 1); // K leading 1's
    bitmask.resize(N, 0); // N-K trailing 0's
    std::vector<std::vector<int>> out;

    // output integers and permute bitmask
    do {
        std::vector<int> comb;
        for (int i = 0; i < N; ++i) { // [0..N-1] integers
            if (bitmask[i]) {
                comb.push_back(i);
            }
        }
        if (f(comb)) {
            out.emplace_back(std::move(comb));
        }
    } while (std::prev_permutation(bitmask.begin(), bitmask.end()));

    return out;
}