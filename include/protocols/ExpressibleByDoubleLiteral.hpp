#pragma once

#include "literal_protocol.hpp"

namespace typecheck {
	class ExpressibleByDoubleLiteral : public LiteralProtocol {
	public:
		ExpressibleByDoubleLiteral() : LiteralProtocol() {}
		~ExpressibleByDoubleLiteral() = default;

		std::vector<Type> getPreferredTypes() const {
			return { ty("double") };
		}

		std::vector<Type> getOtherTypes() const {
			return {};
		}
	};
}