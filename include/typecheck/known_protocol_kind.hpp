#pragma once

#include <variant>

namespace typecheck {
	class KnownProtocolKind {
	public:
		enum DefaultProtocol {
			Sequence = 0,
			IteratorProtocol,
			RawRepresentable,
			Equatable,
			Hashable,
			Comparable,
			Error,
			OptionSet,
			CaseIterable,

			CodingKey,
			Encodable,
			Decodable,

			StringInterpolation,
		};

		enum LiteralProtocol {
			// Start of Literal Protocols
			ExpressibleByArray = 0,
			ExpressibleByBoolean,
			ExpressibleByDictionary,
			ExpressibleByFloat,
			ExpressibleByInteger,
			// ExpressibleByStringInterpolation,
			ExpressibleByString,
			ExpressibleByNil,
			// ExpressibleByUnicodeScalar,

			// ExpressibleByColour,
			// ExpressibleByImage,
			// ExpressibleByFile,
		};

		KnownProtocolKind() : data(false) {}
		KnownProtocolKind(const DefaultProtocol& d) : data(d) {}
		KnownProtocolKind(const LiteralProtocol& l) : data(l) {}
		~KnownProtocolKind() = default;

		const LiteralProtocol& literal() const;
		void set_literal(const LiteralProtocol& literal);
	private:
		std::variant<DefaultProtocol, LiteralProtocol, bool> data;
	};
}
