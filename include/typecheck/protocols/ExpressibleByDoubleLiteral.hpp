#pragma once

#include "typecheck/literal_protocol.hpp"

namespace typecheck {
	class ExpressibleByDoubleLiteral : public LiteralProtocol {
	public:
		ExpressibleByDoubleLiteral() : LiteralProtocol() {}
		virtual ~ExpressibleByDoubleLiteral() = default;

		std::vector<Type> getPreferredTypes() const noexcept override;
		std::vector<Type> getOtherTypes() const noexcept override;
	};
}
