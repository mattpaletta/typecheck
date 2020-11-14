#pragma once
#include <string>		// for string

namespace typecheck {
	void _check(const bool b, const std::string& msg, const std::string& file, const int line);
}

// Begin custom compare Protobuf
template<class T>
inline auto proto_equal(const T& msg_a, const T& msg_b) -> bool {
	return msg_a == msg_b;
}

template<class T>
inline auto proto_not_equal(const T& msg_a, const T& msg_b) -> bool { return !proto_equal(msg_a, msg_b); }
// End custom compare Protobuf

#define TYPECHECK_ASSERT(b, msg) typecheck::_check(b, msg, __FILE__, __LINE__)
