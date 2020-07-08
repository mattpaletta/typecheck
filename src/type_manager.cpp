#include "type_manager.hpp"
#include "debug.hpp"

#include <typecheck_protos/constraint.pb.h>
#include <algorithm>

typecheck::TypeManager::TypeManager() {}

bool typecheck::TypeManager::registerType(const std::string& name) {
	// Determine if has type
	const auto alreadyHasType = this->hasRegisteredType(name);
	if (!alreadyHasType) {
		typecheck::Type type;
		type.set_name(name);
		this->registeredTypes.emplace_back(type);
	}
	return !alreadyHasType;
}

bool typecheck::TypeManager::hasRegisteredType(const std::string& name) const noexcept {
	return !this->getRegisteredType(name).name().empty();
}

typecheck::Type typecheck::TypeManager::getRegisteredType(const std::string& name) const noexcept {
	for (auto& ty : this->registeredTypes) {
		if (ty.name() == name) {
			return ty;
		}
	}
	return typecheck::Type();
}

bool typecheck::TypeManager::setConvertible(const std::string& T0, const std::string& T1) {
	if (T0 == T1) {
		return true;
	}

	const auto t0_ptr = this->getRegisteredType(T0);
	const auto t1_ptr = this->getRegisteredType(T1);

	if (!t0_ptr.name().empty() && !t1_ptr.name().empty() && this->convertible[t0_ptr.name()].find(t1_ptr.name()) == this->convertible[t0_ptr.name()].end()) {
		// Convertible from T0 -> T1
		this->convertible[t0_ptr.name()].insert(t1_ptr.name());
		return true;
	}
	return false;
}

typecheck::Type typecheck::TypeManager::getResolvedType(const typecheck::Type& type) const {
	return this->solver.getResolvedType(type);
}

bool typecheck::TypeManager::isConvertible(const std::string& T0, const std::string& T1) const noexcept {
	if (T0 == T1) {
		return true;
	}

	const auto t0_ptr = this->getRegisteredType(T0);
	const auto t1_ptr = this->getRegisteredType(T1);

	if (this->convertible.find(T0) == this->convertible.end()) {
		// T0 is not in the map, meaning the conversion won't be there.
		return false;
	}

	if (!t0_ptr.name().empty() && !t1_ptr.name().empty() && this->convertible.at(T0).find(t1_ptr.name()) != this->convertible.at(T0).end()) {
		// Convertible from T0 -> T1
		return true;
	}
	return false;
}

void typecheck::TypeManager::registerFunctionDefinition(const typecheck::FunctionDefinition& funcDef) {
	this->functions.push_back(funcDef);
}

std::string typecheck::TypeManager::CreateTypeVar() {
	const auto var = this->type_generator.next();
	this->registerType(var);
	return var;
}

typecheck::Constraint getNewBlankConstraint(typecheck::ConstraintKind kind) {
	typecheck::Constraint constraint;
	constraint.set_kind(kind);
	constraint.set_hasrestriction(false);
	constraint.set_isactive(false);
	constraint.set_isdisabled(false);
	constraint.set_isfavoured(false);
	return constraint;
}

std::size_t typecheck::TypeManager::CreateEqualsConstraint(const typecheck::Type& t0, const typecheck::Type& t1) {
	auto constraint = getNewBlankConstraint(typecheck::ConstraintKind::Equal);
	constraint.set_id(this->constraint_generator.next_id());

	TYPECHECK_ASSERT(!t0.name().empty(), "Cannot use empty type when creating constraint.");
	TYPECHECK_ASSERT(!t1.name().empty(), "Cannot use empty type when creating constraint.");

	TYPECHECK_ASSERT(this->hasRegisteredType(t0.name()), "Must register types before creating constraint");
	TYPECHECK_ASSERT(this->hasRegisteredType(t1.name()), "Must register types before creating constraint");

	constraint.mutable_types()->mutable_first()->set_name(t0.name());
	constraint.mutable_types()->mutable_second()->set_name(t1.name());

	this->constraints.emplace_back(constraint);
	return constraint.id();
}

std::size_t typecheck::TypeManager::CreateConformsToConstraint(const typecheck::Type& t0, const typecheck::KnownProtocolKind& conforms) {
	auto constraint = getNewBlankConstraint(typecheck::ConstraintKind::ConformsTo);

	TYPECHECK_ASSERT(!t0.name().empty(), "Cannot use empty type when creating constraint.");
	TYPECHECK_ASSERT(this->hasRegisteredType(t0.name()), "Must register types before creating constraint");

	constraint.mutable_conforms()->mutable_type()->set_name(t0.name());
	switch (conforms.kind_case()) {
	case typecheck::KnownProtocolKind::kLiteral:
		constraint.mutable_conforms()->mutable_protocol()->set_literal(conforms.literal());
		break;
	case typecheck::KnownProtocolKind::kDefault:
		constraint.mutable_conforms()->mutable_protocol()->set_default_(conforms.default_());
		break;
	case typecheck::KnownProtocolKind::KIND_NOT_SET:
		TYPECHECK_ASSERT(false, "Must set Protocol Kind.");
	}
	constraint.set_id(this->constraint_generator.next_id());
	
	this->constraints.emplace_back(constraint);
	return constraint.id();
}

void typecheck::TypeManager::solve() {
	this->solver.setConstraints(this->constraints);
	//std::vector<typecheck::Type> tmp;
	//std::transform(this->registeredTypes.begin(), this->registeredTypes.end(), tmp.begin(), [](const Type& original) {
	//	// Copy to new.
	//	return original;
	//});
	this->solver.setTypes(this->registeredTypes);
	this->solver.setFunctions(this->functions);
	this->solver.solve();
}
