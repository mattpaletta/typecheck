#include "typecheck/generic_type_generator.hpp"
#include <vector>
#include <algorithm>

// Convert from base 10 to base 26
std::vector<int> convertToBase(const std::size_t init, const std::size_t base) {
	auto curr_div = init;
	std::vector<int> out;
	do {
		auto rem = curr_div % base;
		out.push_back(rem);
		curr_div /= base;
	} while (curr_div > 0);

	std::reverse(out.begin(), out.end());
	return out;
}

std::size_t typecheck::GenericTypeGenerator::next_id() {
	return this->curr_num++;
}

std::string typecheck::GenericTypeGenerator::next() {
	constexpr int A_in_ascii = 65;
		
	std::string out;
	for (const auto ch : convertToBase(this->curr_num++, 26)) {
		out += static_cast<char>(ch + A_in_ascii);
	}
	return out;
}