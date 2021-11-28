#include <typecheck/known_protocol_kind.hpp>
#include <string>

using namespace typecheck;

auto literal_to_str(const KnownProtocolKind::LiteralProtocol& literal) -> std::string {
	switch (literal) {
		case KnownProtocolKind::ExpressibleByDouble:
			return "ExpressibleByDouble";
		case KnownProtocolKind::ExpressibleByArray:
			return "ExpressibleByArray";
		case KnownProtocolKind::ExpressibleByBoolean:
			return "ExpressibleByBoolean";
		case KnownProtocolKind::ExpressibleByDictionary:
			return "ExpressibleByDictionary";
		case KnownProtocolKind::ExpressibleByFloat:
			return "ExpressibleByFloat";
		case KnownProtocolKind::ExpressibleByInteger:
			return "ExpressibleByInteger";
		case KnownProtocolKind::ExpressibleByString:
			return "ExpressibleByString";
		case KnownProtocolKind::ExpressibleByNil:
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
		out += "\"literal\": \"" + literal_to_str(this->literal()) + "\"";
	}
	out += "}";
	return out;
}
