#include "typecheck/generic_type_generator.hpp"
#include <algorithm>  // for reverse
#include <string>     // for basic_string
#include <vector>     // for vector<>::iterator, vector

using namespace typecheck;

auto GenericTypeGenerator::next_id() -> long long {
	return this->curr_num++;
}

auto GenericTypeGenerator::next() -> std::string {
    return "T" + std::to_string(this->next_id());
}
