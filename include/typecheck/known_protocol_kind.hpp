#pragma once

#include <variant>
#include <string>

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

		KnownProtocolKind() : _data(false) {}
		KnownProtocolKind(const DefaultProtocol& d) : _data(d) {}
		KnownProtocolKind(const LiteralProtocol& l) : _data(l) {}
		~KnownProtocolKind() = default;

		const LiteralProtocol& literal() const;
		void set_literal(const LiteralProtocol& literal);
		bool has_literal() const;

		std::string ShortDebugString() const;
	private:
		std::variant<DefaultProtocol, LiteralProtocol, bool> _data;
	};
}
