#include "typecheck/type_manager.hpp"
#include "typecheck/debug.hpp"

#include "typecheck/resolvers/ResolveConformsTo.hpp"
#include "typecheck/resolvers/ResolveEquals.hpp"

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
	for (auto& type : this->registeredTypes) {
		if (type.name() == name) {
			return type;
		}
	}

	return {};
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

typecheck::Type typecheck::TypeManager::getResolvedType(const typecheck::TypeVar& type) const {
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

bool typecheck::TypeManager::registerResolver(std::unique_ptr<Resolver>&& resolver) {
	bool will_insert = this->registeredResolvers.find(resolver->kind) == this->registeredResolvers.end();
	if (will_insert) {
		// We don't have one yet, so add it
		this->registeredResolvers.emplace(std::make_pair(resolver->kind, std::move(resolver)));
	}

	return will_insert;
}

typecheck::TypeVar typecheck::TypeManager::CreateTypeVar() {
	const auto var = this->type_generator.next();

	this->registeredTypeVars.insert(var);

	TypeVar type;
	type.set_symbol(var);
	return type;
}

const typecheck::Constraint& typecheck::TypeManager::getConstraint(const std::size_t id) const {
	for (auto& constraint : this->constraints) {
		if (constraint.id() == id) {
			return constraint;
		}
	}

	return {};
}

bool typecheck::TypeManager::solve() {
	// Add default `resolvers`, ignore response
	// it will not double-register, so this is safe
	constexpr auto default_id = std::numeric_limits<std::size_t>::max();
	this->registerResolver(std::make_unique<typecheck::ResolveConformsTo>(nullptr, default_id));
	this->registerResolver(std::make_unique<typecheck::ResolveEquals>(nullptr, default_id));

	// Finally, solve
	return this->solver.solve(this);
}
