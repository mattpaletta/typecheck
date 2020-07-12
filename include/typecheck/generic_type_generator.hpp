#pragma once
#include <string>

namespace typecheck {
	class GenericTypeGenerator {
	private:
		std::size_t curr_num = 0;
	public:
		std::string next();
		std::size_t next_id();
		GenericTypeGenerator() = default;
		~GenericTypeGenerator() = default;
	};
}