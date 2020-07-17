#include "typecheck/generic_type_generator.hpp"
#include <algorithm>  // for reverse
#include <string>     // for basic_string
#include <vector>     // for vector<>::iterator, vector

auto typecheck::GenericTypeGenerator::next_id() -> long long {
	return this->curr_num++;
}

auto typecheck::GenericTypeGenerator::next() -> std::string {
    return "T" + std::to_string(this->next_id());
}
