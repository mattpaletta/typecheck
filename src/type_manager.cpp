#include "typecheck/type_manager.hpp"
#include <typecheck_protos/constraint.pb.h>           // for ConstraintKind
#include <limits>                                     // for numeric_limits
#include <type_traits>                                // for move
#include <utility>                                    // for make_pair
#include "typecheck/generic_type_generator.hpp"       // for GenericTypeGene...

#include "typecheck/resolver.hpp"                     // for Resolver
#include "typecheck/resolvers/ResolveConformsTo.hpp"  // for ResolveConformsTo
#include "typecheck/resolvers/ResolveEquals.hpp"      // for ResolveEquals
#include "typecheck/resolvers/ResolveConvertible.hpp" // for ResolveConvertible
#include "typecheck/resolvers/ResolveApplicableFunction.hpp" // for ResolveApplicableFunction
#include "typecheck/resolvers/ResolveBindOverload.hpp" // for ResolveBindOverload
#include "typecheck/resolvers/ResolveBindTo.hpp"       // for ResolveBindTo

#include "typecheck/type_solver.hpp"                  // for TypeSolver
#include <typecheck_protos/type.pb.h>                 // for Type, TypeVar

#include <google/protobuf/util/message_differencer.h>

typecheck::TypeManager::TypeManager() {}

bool typecheck::TypeManager::registerType(const std::string& name) {
    Type ty;
    ty.mutable_raw()->set_name(name);
    return this->registerType(ty);
}

bool typecheck::TypeManager::registerType(const Type& name) {
	// Determine if has type
	const auto alreadyHasType = this->hasRegisteredType(name);
	if (!alreadyHasType) {
		typecheck::Type type;
		type.CopyFrom(name);
		this->registeredTypes.emplace_back(type);
	}
	return !alreadyHasType;
}

bool typecheck::TypeManager::hasRegisteredType(const std::string& name) const noexcept {
    const auto returned = this->getRegisteredType(name);
    return returned.has_raw() || returned.has_func();
}

bool typecheck::TypeManager::hasRegisteredType(const Type& name) const noexcept {
    const auto returned = this->getRegisteredType(name);
    return returned.has_raw() || returned.has_func();
}

typecheck::Type typecheck::TypeManager::getRegisteredType(const std::string& name) const noexcept {
    Type ty;
    ty.mutable_raw()->set_name(name);
    return this->getRegisteredType(ty);
}

typecheck::Type typecheck::TypeManager::getRegisteredType(const Type& name) const noexcept {
	for (auto& type : this->registeredTypes) {
        if (google::protobuf::util::MessageDifferencer::Equals(type, name)) {
			return type;
		}
	}

	return {};
}

std::vector<typecheck::Type> typecheck::TypeManager::getFunctionOverloads(const TypeVar& var) const {
    std::vector<typecheck::Type> overloads;
    for (auto& constraint : this->constraints) {
        // Lookup by 'var', to deal with anonymous functions.
        if (constraint.kind() == ConstraintKind::ApplicableFunction &&
            google::protobuf::util::MessageDifferencer::Equals(constraint.explicit_().var(), var)) {
            overloads.push_back(constraint.explicit_().type());
        }
    }

    return overloads;
}

bool typecheck::TypeManager::setConvertible(const std::string& T0, const std::string& T1) {
    Type t0;
    t0.mutable_raw()->set_name(T0);

    Type t1;
    t1.mutable_raw()->set_name(T1);

    return this->setConvertible(t0, t1);
}

bool typecheck::TypeManager::setConvertible(const Type& T0, const Type& T1) {
    if (google::protobuf::util::MessageDifferencer::Equals(T0, T1)) {
		return true;
	}

	const auto t0_ptr = this->getRegisteredType(T0);
	const auto t1_ptr = this->getRegisteredType(T1);

    // Function types not convertible
    if (t0_ptr.has_func() || t1_ptr.has_func()) {
        // Functions not convertible to each other
        return false;
    } else if (!t0_ptr.raw().name().empty() && !t1_ptr.raw().name().empty() && this->convertible[t0_ptr.raw().name()].find(t1_ptr.raw().name()) == this->convertible[t0_ptr.raw().name()].end()) {
		// Convertible from T0 -> T1
        this->convertible[t0_ptr.raw().name()].insert(t1_ptr.raw().name());
		return true;
	}
	return false;
}

typecheck::Type typecheck::TypeManager::getResolvedType(const typecheck::TypeVar& type) const {
	return this->solver.getResolvedType(type);
}

bool typecheck::TypeManager::isConvertible(const std::string& T0, const std::string& T1) const noexcept {
    Type t0;
    t0.mutable_raw()->set_name(T0);

    Type t1;
    t1.mutable_raw()->set_name(T1);
    return this->isConvertible(t0, t1);
}

bool typecheck::TypeManager::isConvertible(const Type& T0, const Type& T1) const noexcept {
    if (google::protobuf::util::MessageDifferencer::Equals(T0, T1)) {
		return true;
	}

	const auto t0_ptr = this->getRegisteredType(T0);
	const auto t1_ptr = this->getRegisteredType(T1);

    // Function types not convertible
    if (t0_ptr.has_func() || t1_ptr.has_func()) {
        return false;
    }

    if (this->convertible.find(T0.raw().name()) == this->convertible.end()) {
		// T0 is not in the map, meaning the conversion won't be there.
		return false;
	}

    if (!t0_ptr.raw().name().empty() && !t1_ptr.raw().name().empty() && \
        this->convertible.at(T0.raw().name()).find(t1_ptr.raw().name()) != this->convertible.at(T0.raw().name()).end()) {
		// Convertible from T0 -> T1
		return true;
	}
	return false;
}

std::vector<typecheck::Type> typecheck::TypeManager::getConvertible(const Type& T0) const {
    std::vector<Type> out;

    // Function types not convertible
    if (T0.has_func()) {
        return out;
    }

    if (this->convertible.find(T0.raw().name()) != this->convertible.end()) {
        // Load into vector
        for (auto& convert : this->convertible.at(T0.raw().name())) {
            Type type;
            type.mutable_raw()->set_name(convert);
            out.emplace_back(std::move(type));
        }
    }

    return out;
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

const typecheck::Constraint* typecheck::TypeManager::getConstraint(const IDType id) const {
	for (auto& constraint : this->constraints) {
		if (constraint.id() == id) {
			return &constraint;
		}
	}

	return nullptr;
}

bool typecheck::TypeManager::solve() {
	// Add default `resolvers`, ignore response
	// it will not double-register, so this is safe
	constexpr auto default_id = std::numeric_limits<std::size_t>::max();
	this->registerResolver(std::make_unique<typecheck::ResolveConformsTo>(nullptr, default_id));
	this->registerResolver(std::make_unique<typecheck::ResolveEquals>(nullptr, default_id));
    this->registerResolver(std::make_unique<typecheck::ResolveConvertible>(nullptr, default_id));
    this->registerResolver(std::make_unique<typecheck::ResolveApplicableFunction>(nullptr, default_id));
    this->registerResolver(std::make_unique<typecheck::ResolveBindOverload>(nullptr, default_id));
    this->registerResolver(std::make_unique<typecheck::ResolveBindTo>(nullptr, default_id));

    this->SortConstraints();

	// Finally, solve
	return this->solver.solve(this);
}
