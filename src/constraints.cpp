#include <typecheck/constraint.hpp>

#include <variant>
#include <sstream>
#include <string>

using namespace typecheck;

Constraint::Conforms::Conforms() = default;

Constraint::Conforms::Conforms(Conforms&& other) noexcept : _protocol(other._protocol), _type(std::move(other._type)) {}

Constraint::Conforms::Conforms(const Conforms& other)  = default;

auto Constraint::Conforms::operator=(Conforms&& other) noexcept -> Conforms& {
	if (other._protocol.has_value()) {
		this->_protocol = other._protocol;
	}

	if (other._type.has_value()) {
		this->_type = std::move(other._type);
	}
	return *this;
}

auto Constraint::Conforms::has_type() const -> bool {
	return this->_type.has_value();
}

auto Constraint::Conforms::type() const -> const TypeVar& {
	if (!this->has_type()) {
		this->_type = TypeVar{};
	}
	return this->_type.value();
}

auto Constraint::Conforms::mutable_type() -> TypeVar* {
	if (!this->has_type()) {
		this->_type = TypeVar{};
	}
	return &this->_type.value();
}

auto Constraint::Conforms::protocol() const -> const KnownProtocolKind& {
	if (!this->has_protocol()) {
		this->_protocol = KnownProtocolKind{};
	}
	return this->_protocol.value();
}

auto Constraint::Conforms::has_protocol() const -> bool {
	return this->_protocol.has_value();
}

auto Constraint::Conforms::mutable_protocol() -> KnownProtocolKind* {
	if (!this->has_protocol()) {
		this->_protocol = KnownProtocolKind{};
	}
	return &this->_protocol.value();
}

auto Constraint::Conforms::ShortDebugString() const -> std::string {
	std::string out;
	out += "{ ";
	if (this->has_type()) {
		out += "\"type\": " + this->_type->ShortDebugString() + (this->has_protocol() ? ", " : " ");
	}
	if (this->has_protocol()) {
		out += "\"protocol\": " + this->_protocol->ShortDebugString() + " ";
	}
	out += "}";
	return out;
}

Constraint::Types::Types() = default;

Constraint::Types::Types(Types&& other) noexcept : _first(std::move(other._first)), _second(std::move(other._second)), _third(std::move(other._third)) {}

auto Constraint::Types::operator=(Types&& other) noexcept -> Types& {
	this->_first = std::move(other._first);
	this->_second = std::move(other._second);
	this->_third = std::move(other._third);
	return *this;
}

Constraint::Types::Types(const Types& other)  = default;

auto Constraint::Types::has_first() const -> bool {
	return this->_first.has_value();
}

auto Constraint::Types::first() const -> const TypeVar& {
	if (!this->has_first()) {
		this->_first = TypeVar{};
	}
	return this->_first.value();
}

auto Constraint::Types::mutable_first() -> TypeVar* {
	if (!this->has_first()) {
		this->_first = TypeVar{};
	}
	return &this->_first.value();
}

auto Constraint::Types::has_second() const -> bool {
	return this->_second.has_value();
}

auto Constraint::Types::second() const -> const TypeVar& {
	if (!this->has_second()) {
		this->_second = TypeVar{};
	}
	return this->_second.value();
}

auto Constraint::Types::mutable_second() -> TypeVar* {
	if (!this->has_second()) {
		this->_second = TypeVar{};
	}
	return &this->_second.value();
}

auto Constraint::Types::has_third() const -> bool {
	return this->_third.has_value();
}

auto Constraint::Types::third() const -> const TypeVar& {
	if (!this->has_third()) {
		this->_third = TypeVar{};
	}
	return this->_third.value();
}

auto Constraint::Types::mutable_third() -> TypeVar* {
	if (!this->has_third()) {
		this->_third = TypeVar{};
	}
	return &this->_third.value();
}

auto Constraint::Types::ShortDebugString() const -> std::string {
	std::string out;
	out += "{ ";
	if (this->has_first()) {
		out += "\"first\": " + this->_first->ShortDebugString() + (this->has_second() ? ", " : " ");
	}
	if (this->has_second()) {
		out += "\"second\": " + this->_second->ShortDebugString() + (this->has_third() ? ", " : " ");
	}
	if (this->has_third()) {
		out += "\"third\": " + this->_third->ShortDebugString() + " ";
	}
	out += "}";
	return out;
}

Constraint::ExplicitType::ExplicitType() = default;

Constraint::ExplicitType::ExplicitType(ExplicitType&& other) noexcept : _var(std::move(other._var)), _type(std::move(other._type)) {}

Constraint::ExplicitType::ExplicitType(const ExplicitType& other)  = default;

auto Constraint::ExplicitType::operator=(ExplicitType&& other) noexcept -> Constraint::ExplicitType& {
	this->_var = std::move(other._var);
	this->_type = std::move(other._type);
	return *this;
}

auto Constraint::ExplicitType::mutable_var() -> TypeVar* {
	if (!this->has_var()) {
		this->_var = TypeVar{};
	}
	return &this->_var.value();
}

auto Constraint::ExplicitType::has_var() const -> bool {
	return this->_var.has_value();
}

auto Constraint::ExplicitType::var() const -> const TypeVar& {
	if (!this->has_var()) {
		this->_var = TypeVar{};
	}
	return this->_var.value();
}

auto Constraint::ExplicitType::mutable_type() -> Type* {
	if (!this->has_type()) {
		this->_type = Type{};
	}
	return &this->_type.value();
}

auto Constraint::ExplicitType::type() const -> const Type& {
	if (!this->has_type()) {
		this->_type = Type{};
	}
	return this->_type.value();
}
auto Constraint::ExplicitType::has_type() const -> bool {
	return this->_type.has_value();
}

auto Constraint::ExplicitType::ShortDebugString() const -> std::string {
	std::string out;
	out += "{ ";
	if (this->has_type()) {
		out += "\"type\": " + this->_type->ShortDebugString() + (this->has_var() ? ", " : " ");
	}
	if (this->has_var()) {
		out += "\"var\": " + this->_var->ShortDebugString() + " ";
	}
	out += "}";
	return out;
}

Constraint::Overload::Overload() = default;

Constraint::Overload::Overload(Overload&& other) noexcept : _type(std::move(other._type)), _functionID(other._functionID), _argVars(std::move(other._argVars)), _returnVar(other._returnVar) {}

auto Constraint::Overload::operator=(Overload&& other) noexcept -> Overload& {
	this->_type = std::move(other._type);
	this->_functionID = other._functionID;
	this->_argVars = std::move(other._argVars);
	this->_returnVar = other._returnVar;
	return *this;
}

Constraint::Overload::Overload(const Overload& other) : _type(other.type()), _functionID(other.functionid()), _argVars(other._argVars), _returnVar(other._returnVar) {}

auto Constraint::Overload::operator=(const Overload& other) -> Overload& {
	if (this == &other) {
		return *this;
	}

	this->_type = other.type();
	this->_functionID = other.functionid();
	this->_argVars = other._argVars;
	this->_returnVar = other._returnVar;
	return *this;
}

auto Constraint::Overload::add_argvars() -> TypeVar* {
	this->_argVars.emplace_back();
	return &this->_argVars.at(this->_argVars.size() - 1);
}

auto Constraint::Overload::argvars_size() const -> int {
	return static_cast<int>(this->_argVars.size());
}

auto Constraint::Overload::argvars(const int i) const -> const TypeVar& {
	return this->_argVars.at(static_cast<std::size_t>(i));
}

auto Constraint::Overload::mutable_type() -> TypeVar* {
	if (!this->has_type()) {
		this->_type = TypeVar{};
	}
	return &this->_type.value();
}

auto Constraint::Overload::has_type() const -> bool {
	return this->_type.has_value();
}

auto Constraint::Overload::type() const -> const TypeVar& {
	if (!this->has_type()) {
		this->_type = TypeVar{};
	}
	return this->_type.value();
}

auto Constraint::Overload::functionid() const -> long long {
	return this->_functionID;
}

void Constraint::Overload::set_functionid(const long long functionid) {
	this->_functionID = functionid;
}

auto Constraint::Overload::mutable_returnvar() -> TypeVar* {
	return &this->_returnVar;
}

auto Constraint::Overload::returnvar() const -> const TypeVar& {
	return this->_returnVar;
}

auto Constraint::Overload::ShortDebugString() const -> std::string {
	std::string out;
	out += "{ ";
	out += "\"returnVar\": " + this->_returnVar.ShortDebugString() + ", ";
	out += "\"functionID\": " + std::to_string(this->_functionID) + ", ";
	out += "\"type\": " + this->_type->ShortDebugString() + ", ";
	out += "\"argVars\": [";
	for (const auto& arg : this->_argVars) {
		out += (arg.ShortDebugString() + (!(arg == this->_argVars.at(this->_argVars.size() - 1)) ? ", " : " "));
	}
	out += "]";
	out += "}";
	return out;
}

Constraint::Constraint() : _kind() {}

auto Constraint::has_explicit_() const -> bool {
	return std::holds_alternative<ExplicitType>(this->data);
}

auto Constraint::explicit_() const -> const ExplicitType& {
	if (!this->has_explicit_()) {
		this->data = ExplicitType{};
	}
	return std::get<ExplicitType>(this->data);
}

auto Constraint::mutable_explicit_() -> ExplicitType* {
	if (!this->has_explicit_()) {
		this->data = ExplicitType{};
	}
	return &std::get<ExplicitType>(this->data);
}

auto Constraint::has_overload() const -> bool {
	return std::holds_alternative<Overload>(this->data);
}

auto Constraint::overload() const -> const Overload& {
	if (!this->has_overload()) {
		this->data = Overload{};
	}
	return std::get<Overload>(this->data);
}

auto Constraint::mutable_overload() -> Overload* {
	if (!this->has_overload()) {
		this->data = Overload{};
	}
	return &std::get<Overload>(this->data);
}

auto Constraint::has_types() const -> bool {
	return std::holds_alternative<Types>(this->data);
}

auto Constraint::types() const -> const Types& {
	if (!this->has_types()) {
		this->data = Types{};
	}
	return std::get<Types>(this->data);
}

auto Constraint::mutable_types() -> Types* {
	if (!this->has_types()) {
		this->data = Types{};
	}
	return &std::get<Types>(this->data);
}

auto Constraint::has_conforms() const -> bool {
	return std::holds_alternative<Conforms>(this->data);
}

auto Constraint::conforms() const -> const Conforms& {
	if (!this->has_conforms()) {
		this->data = Conforms{};
	}
	return std::get<Conforms>(this->data);
}

auto Constraint::mutable_conforms() -> Conforms* {
	if (!this->has_conforms()) {
		this->data = Conforms{};
	}
	return &std::get<Conforms>(this->data);
}
void Constraint::set_id(const long long id) {
	this->_id = id;
}

auto Constraint::id() const -> long long {
	return this->_id;
}

void Constraint::set_kind(const ConstraintKind& kind) {
	this->_kind = kind;
}

auto Constraint::kind() const -> const ConstraintKind& {
	return this->_kind;
}

auto Constraint::ShortDebugString() const -> std::string {
	std::string out;
	out += "{ \"id\": " + std::to_string(this->_id) + ", ";
	if (this->has_types()) {
		out += ("\"types\": \t" + this->types().ShortDebugString() + (" "));
	} else if (this->has_conforms()) {
		out += ("\"conforms\": \t" + this->conforms().ShortDebugString() + " ");
	} else if (this->has_overload()) {
		out += ("\"overload\": \t" + this->overload().ShortDebugString() + " ");
	} else if (this->has_explicit_()) {
		out += ("\"explicit\": \t" + this->explicit_().ShortDebugString()+ " ");
	} else {
		return "Unknown Constraint Type";
	}

	out += " }";
	return out;
}
