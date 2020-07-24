#pragma once

#include "typecheck/literal_protocol.hpp"

namespace typecheck {
	class ExpressibleByIntegerLiteral : public LiteralProtocol {
	public:
		ExpressibleByIntegerLiteral() : LiteralProtocol() {}
		virtual ~ExpressibleByIntegerLiteral() = default;

		std::vector<Type> getPreferredTypes() const noexcept override;
		std::vector<Type> getOtherTypes() const noexcept override;
	};
}
