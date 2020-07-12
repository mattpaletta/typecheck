#pragma once

#include "typecheck/literal_protocol.hpp"

namespace typecheck {
	class ExpressibleByIntegerLiteral : public LiteralProtocol {
	public:
		ExpressibleByIntegerLiteral() : LiteralProtocol() {}
		~ExpressibleByIntegerLiteral() = default;

		std::vector<Type> getPreferredTypes() const;
		std::vector<Type> getOtherTypes() const;
	};
}
