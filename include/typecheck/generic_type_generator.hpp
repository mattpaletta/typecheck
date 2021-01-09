#pragma once

#include <string>

namespace typecheck {
	class GenericTypeGenerator {
	private:
		long long curr_num = 0;
	public:
		std::string next();
        long long next_id();
		GenericTypeGenerator() = default;
		~GenericTypeGenerator() = default;
	};
}
