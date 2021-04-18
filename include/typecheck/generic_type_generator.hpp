#pragma once

#include <string>

namespace typecheck {
	class GenericTypeGenerator {
	public:
        using value_type = long long;
		GenericTypeGenerator() = default;
		~GenericTypeGenerator() = default;

        std::string next();
        value_type next_id();

    private:
        value_type curr_num = 0;
	};
}
