#include <typecheck/known_protocol_kind.hpp>

using namespace typecheck;

auto KnownProtocolKind::literal() const -> const LiteralProtocol& {
	return std::get<LiteralProtocol>(this->data);
}

void KnownProtocolKind::set_literal(const LiteralProtocol& literal) {
	this->data = literal;
}
