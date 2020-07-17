#include "typecheck/generic_type_generator.hpp"
#include <algorithm>  // for reverse
#include <string>     // for basic_string
#include <vector>     // for vector<>::iterator, vector

long long typecheck::GenericTypeGenerator::next_id() {
	return this->curr_num++;
}

std::string typecheck::GenericTypeGenerator::next() {
    return "T" + std::to_string(this->next_id());
}
