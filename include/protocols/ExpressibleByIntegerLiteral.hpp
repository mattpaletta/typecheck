#pragma once

#include "literal_protocol.hpp"

namespace typecheck {
	class ExpressibleByIntegerLiteral : public LiteralProtocol {
	public:
		ExpressibleByIntegerLiteral() : LiteralProtocol() {}
		~ExpressibleByIntegerLiteral() = default;

		std::vector<Type> getPreferredTypes() const {
			return { ty("int") };
		}

		std::vector<Type> getOtherTypes() const {
			return { ty("double"), ty("float") };
		}
	};
}