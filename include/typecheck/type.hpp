#pragma once

#include "raw_type.hpp"
#include "function_definition.hpp"

#include <variant>

namespace typecheck {
	class Type {
	public:
		Type();
		Type(const RawType& r);
		Type(const FunctionDefinition& f);
		~Type() = default;

		Type(const Type& other);
		Type& operator=(const Type& other);
		Type(Type&& other) noexcept;
		Type& operator=(Type&& other) noexcept;

		bool operator==(const Type& other) const noexcept;

		Type& CopyFrom(const Type& other);

		bool has_raw() const;
		bool has_func() const;

		RawType* mutable_raw();
		const RawType& raw() const;

		FunctionDefinition* mutable_func();
		const FunctionDefinition& func() const;
	private:
		// Variant defaults to the first type
		mutable std::variant<bool, RawType, FunctionDefinition> data;
	};
}
