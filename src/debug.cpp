#include "typecheck/debug.hpp"

#include <iostream>
#include <stdexcept>  // for runtime_error
#include <string>     // for char_traits

void typecheck::_check(const bool b, const std::string& msg, const std::string& file, const int line) {
    // LCOV_EXCL_START
	if (!b) {
		std::cout << "Error occurred: [" << file << ":" << line << "]" << std::endl;
		throw std::runtime_error(msg);
	}
    // LCOV_EXCL_STOP
}