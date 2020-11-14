#include <typecheck/type.hpp>

using namespace typecheck;


FunctionDefinition::FunctionDefinition()  = default;

FunctionDefinition::FunctionDefinition(const FunctionDefinition& other) {
	this->CopyFrom(other);
}

auto FunctionDefinition::operator=(const FunctionDefinition& other) -> FunctionDefinition& {
	if (this == &other) {
		return *this;
	}

	this->CopyFrom(other);
	return *this;
}

void FunctionDefinition::CopyFrom(const FunctionDefinition& other) {
	this->_args = other._args;
	if (this->has_returntype()) {
		this->_returnType = std::make_unique<Type>(other.returntype());
	}
	this->_name = other._name;
	this->_id = other._id;
}

auto FunctionDefinition::operator==(const FunctionDefinition& other) const noexcept -> bool {
	return this->args_size() == other.args_size() &&
		this->_returnType == other._returnType &&
		this->name() == other.name() &&
		this->id() == other.id() &&
		// This is the most expensive, do it last
		this->_args == other._args;
}

auto FunctionDefinition::mutable_returntype() -> Type* {
	if (!this->has_returntype()) {
		this->_returnType = std::make_unique<Type>();
	}
	return this->_returnType.get();
}

auto FunctionDefinition::has_returntype() const -> bool {
	return this->_returnType.operator bool();
}

auto FunctionDefinition::returntype() const -> const Type& {
	if (!this->has_returntype()) {
		this->_returnType = std::make_unique<Type>();
	}
	return *this->_returnType.get();
}

auto FunctionDefinition::add_args() -> Type* {
	this->_args.emplace_back();
	return &this->_args.at(this->_args.size() - 1);
}

auto FunctionDefinition::args_size() const -> int {
	return static_cast<int>(this->_args.size());
}

auto FunctionDefinition::args(const int i) const -> const Type& {
	return this->_args.at(static_cast<std::size_t>(i));
}

auto FunctionDefinition::name() const -> std::string {
	return this->_name;
}

auto FunctionDefinition::id() const -> int {
	return this->_id;
}
