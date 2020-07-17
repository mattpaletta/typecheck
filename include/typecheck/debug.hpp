#pragma once
#include <iosfwd>		// for string

namespace typecheck {
	void _check(const bool b, const std::string& msg, const std::string& file, const int line);
}

#define TYPECHECK_ASSERT(b, msg) typecheck::_check(b, msg, __FILE__, __LINE__)
