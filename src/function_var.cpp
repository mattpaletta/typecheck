#include <typecheck/function_var.hpp>
#include <typecheck/type_var.hpp>

using namespace typecheck;

FunctionVar::FunctionVar() : _id(0) {}

auto FunctionVar::id() const -> long long {
	return this->_id;
}

void FunctionVar::set_id(const long long id) {
	this->_id = id;
}

auto FunctionVar::mutable_returnvar() -> TypeVar* {
	return &this->_returnVar;
}

auto FunctionVar::returnvar() const -> const TypeVar& {
	return this->_returnVar;
}

auto FunctionVar::args() const -> const std::vector<TypeVar>& {
	return this->_args;
}

auto FunctionVar::add_args() -> TypeVar* {
	this->_args.emplace_back();
	return &this->_args.at(this->_args.size() - 1);
}
