#include <typecheck/type.hpp>
#include <typecheck/raw_type.hpp>
#include <typecheck/function_definition.hpp>

#include <variant>
#include <iostream>

using namespace typecheck;

Type::Type() : data(false) {}

Type::Type(const RawType& r) : data(r) {}

Type::Type(const FunctionDefinition& f) : data(f) {}

Type::Type(const Type& other)  = default;

auto Type::operator=(const Type& other) -> Type& {
	if (this == &other) {
		return *this;
	}

	this->CopyFrom(other);
	return *this;
}

Type::Type(Type&& other) noexcept : data(std::move(other.data)) {}

auto Type::operator=(Type&& other) noexcept -> Type& {
	if (this == &other) {
		return *this;
	}

	this->data = std::move(other.data);

	return *this;
}

auto Type::operator==(const Type& other) const noexcept -> bool {
	return (this->has_raw() && other.has_raw() && this->raw() == other.raw()) ||
		(this->has_func() && other.has_func() && this->func() == other.func());
}

auto Type::CopyFrom(const Type& other) -> Type& {
	if (this == &other) {
		return *this;
	}

	if (other.has_raw()) {
		this->data = other.raw();
	} else if (other.has_func()) {
		this->data = other.func();
	} else {
		this->data.emplace<bool>(false);
	}

	return *this;
}

auto Type::has_raw() const -> bool {
	return std::holds_alternative<RawType>(this->data);
}

auto Type::has_func() const -> bool {
	return std::holds_alternative<FunctionDefinition>(this->data);
}

auto Type::mutable_raw() -> RawType* {
	if (!this->has_raw()) {
		this->data = RawType{};
	}
	return std::get_if<RawType>(&this->data);
}

auto Type::raw() const -> const RawType& {
	if (!this->has_raw()) {
		this->data = RawType{};
	}
	return std::get<RawType>(this->data);
}

auto Type::mutable_func() -> FunctionDefinition* {
	if (!this->has_func()) {
		this->data = FunctionDefinition{};
	}
	return std::get_if<FunctionDefinition>(&this->data);
}

auto Type::func() const -> const FunctionDefinition& {
	if (!this->has_func()) {
		this->data = FunctionDefinition{};
	}
	return std::get<FunctionDefinition>(this->data);
}
