#include <typecheck/type_var.hpp>
#include <utility>

using namespace typecheck;

TypeVar::TypeVar(std::string s) : _symbol(std::move(s)) {}

auto TypeVar::operator==(const TypeVar& other) const noexcept -> bool {
	return this->_symbol == other._symbol;
}

auto TypeVar::operator!=(const TypeVar& other) const noexcept -> bool {
	return !(*this == other);
}

bool TypeVar::operator<(const TypeVar& other) const noexcept {
	return this->_symbol < other._symbol;
}


void TypeVar::CopyFrom(const TypeVar& other) {
	this->_symbol = other.symbol();
}

auto TypeVar::symbol() const -> std::string {
	return this->_symbol;
}

void TypeVar::set_symbol(const std::string& s) {
	this->_symbol = s;
}

auto TypeVar::ShortDebugString() const -> std::string {
	return "{ \"symbol\": \"" + this->_symbol + "\" }";
}
