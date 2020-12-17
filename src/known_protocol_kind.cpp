#include <typecheck/known_protocol_kind.hpp>
#include <string>

using namespace typecheck;

auto literal_to_str(const KnownProtocolKind::LiteralProtocol& literal) -> std::string {
	switch (literal) {
		case typecheck::KnownProtocolKind::ExpressibleByArray:
			return "ExpressibleByArray";
		case typecheck::KnownProtocolKind::ExpressibleByBoolean:
			return "ExpressibleByBoolean";
		case typecheck::KnownProtocolKind::ExpressibleByDictionary:
			return "ExpressibleByDictionary";
		case typecheck::KnownProtocolKind::ExpressibleByFloat:
			return "ExpressibleByFloat";
		case typecheck::KnownProtocolKind::ExpressibleByInteger:
			return "ExpressibleByInteger";
		case typecheck::KnownProtocolKind::ExpressibleByString:
			return "ExpressibleByString";
		case typecheck::KnownProtocolKind::ExpressibleByNil:
			return "ExpressibleByNil";
	}
}

auto KnownProtocolKind::literal() const -> const LiteralProtocol& {
	return std::get<LiteralProtocol>(this->_data);
}

void KnownProtocolKind::set_literal(const LiteralProtocol& literal) {
	this->_data = literal;
}

auto KnownProtocolKind::has_literal() const -> bool {
	return std::holds_alternative<LiteralProtocol>(this->_data);
}

auto KnownProtocolKind::ShortDebugString() const -> std::string {
	std::string out;
	out += "{ ";
	if (this->has_literal()) {
		out += "{ literal: " + literal_to_str(this->literal());
	}
	out += "}";
	return out;
}
