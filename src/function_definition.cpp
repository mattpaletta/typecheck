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

	if (other.has_returntype()) {
		this->_returnType = std::make_unique<Type>();
		this->_returnType->CopyFrom(other.returntype());
	}

	this->_name = other._name;
	this->_id = other._id;
}

auto FunctionDefinition::operator==(const FunctionDefinition& other) const noexcept -> bool {
	const auto args_same = this->args_size() == other.args_size() &&
		this->name() == other.name() &&
		this->id() == other.id() &&
		this->has_returntype() == other.has_returntype() &&
		// This is the most expensive, do it last
		this->_args == other._args;

	if (args_same && this->has_returntype() && other.has_returntype()) {
		// Break ties by checking return Type, not the unique_ptr
		return this->returntype() == other.returntype();
	} else {
		return args_same;
	}
}

auto FunctionDefinition::operator!=(const FunctionDefinition& other) const noexcept -> bool {
	return !(*this == other);
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

void FunctionDefinition::set_name(const std::string& name) {
    this->_name = name;
}

auto FunctionDefinition::id() const -> int {
	return this->_id;
}

void FunctionDefinition::set_id(int id) {
    this->_id = id;
}

std::string FunctionDefinition::ShortDebugString() const {
	std::string out;
	out += "{ ";
	out += "\"name\": \"" + this->_name + "\", ";
	if (this->has_returntype()) {
		out += "\"returnType\": \"" + this->returntype().ShortDebugString() + "\", ";
	}
	out += "\"id\": " + std::to_string(this->_id) + ", ";
	out += "\"args\": [";
	for (const auto& arg : this->_args) {
		out += arg.ShortDebugString() + (!(arg == this->args(this->args_size() - 1)) ? ", " : " ");
	}
	out += "],";
	out += " }";
	return out;
}
