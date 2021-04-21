#include <typecheck/function_var.hpp>
#include <typecheck/type_var.hpp>

#include <cppnotstdlib/strings.hpp>

#include <sstream>

using namespace typecheck;

FunctionVar::FunctionVar() : _id(0) {}

auto FunctionVar::name() const -> std::string {
    return this->_name;
}

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

auto FunctionVar::serialize() const -> std::string {
    std::stringstream ss;

    if (this->_args.size() == 0) {
        ss << "<empty>";
    } else {
        for (const auto& a : _args) {
            ss << a.symbol() << ",";
        }
    }
    ss << "|";
    ss << (this->_returnVar.symbol().empty() ? "<empty>" : this->_returnVar.symbol());
    ss << "|";
    ss << (this->_name.empty() ? "<empty>" : this->_name);
    ss << "|";
    ss << this->_id;

    return ss.str();
}

auto FunctionVar::unserialize(const std::string& str) -> FunctionVar {
    const auto parts = cppnotstdlib::explode(str, '|');

    const auto args = parts.at(0) == "<empty>" ? std::vector<std::string>{} : cppnotstdlib::explode(parts.at(0), ',');
    const auto returnVar = parts.at(1);
    const auto name = parts.at(2);
    const auto id = std::stol(parts.at(3));

    FunctionVar f;
    for (const auto& a : args) {
        f._args.emplace_back(a);
    }
    f._returnVar = (returnVar == "<empty>" ? "" : returnVar);
    f._name = (name == "<empty>" ? "" : name);
    f._id = id;

    return f;
}
