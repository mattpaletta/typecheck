#pragma once 
#include <string>		// for string

namespace typecheck {
	void _check(const bool b, const std::string& msg, const std::string& file, const int line);
}

// Begin custom compare Protobuf
template<class T>
auto operator==(const T& msg_a, const T& msg_b) -> bool {
	return (msg_a.GetTypeName() == msg_b.GetTypeName()) && (msg_a.DebugString() == msg_b.DebugString());
}

template<class T>
auto operator!=(const T& msg_a, const T& msg_b) -> bool { return !(msg_a == msg_b); }
// End custom compare Protobuf

#define TYPECHECK_ASSERT(b, msg) typecheck::_check(b, msg, __FILE__, __LINE__)
