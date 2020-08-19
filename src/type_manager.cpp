#include "typecheck/type_manager.hpp"
#include <typecheck_protos/constraint.pb.h>           // for ConstraintKind
#include <limits>                                     // for numeric_limits
#include <type_traits>                                // for move
#include <utility>                                    // for make_pair
#include "typecheck/generic_type_generator.hpp"       // for GenericTypeGene...
#include <sstream>                                    // for std::stringstream
#include <string>                                     // for std::string

#include "typecheck/debug.hpp"
#include "typecheck/resolver.hpp"                     // for Resolver
#include "typecheck/resolvers/ResolveConformsTo.hpp"  // for ResolveConformsTo
#include "typecheck/resolvers/ResolveEquals.hpp"      // for ResolveEquals
#include "typecheck/resolvers/ResolveConvertible.hpp" // for ResolveConvertible
#include "typecheck/resolvers/ResolveApplicableFunction.hpp" // for ResolveApplicableFunction
#include "typecheck/resolvers/ResolveBindOverload.hpp" // for ResolveBindOverload
#include "typecheck/resolvers/ResolveBindTo.hpp"       // for ResolveBindTo

#include "typecheck/type_solver.hpp"                  // for TypeSolver
#include <typecheck_protos/type.pb.h>                 // for Type, TypeVar

using namespace typecheck;

TypeManager::TypeManager() = default;

auto TypeManager::registerType(const std::string& name) -> bool {
    Type ty;
    ty.mutable_raw()->set_name(name);
    return this->registerType(ty);
}

auto TypeManager::registerType(const Type& name) -> bool {
	// Determine if has type
	const auto alreadyHasType = this->hasRegisteredType(name);
	if (!alreadyHasType) {
		Type type;
		type.CopyFrom(name);
		this->registeredTypes.emplace_back(type);
	}
	return !alreadyHasType;
}

auto TypeManager::hasRegisteredType(const std::string& name) const noexcept -> bool {
    const auto returned = this->getRegisteredType(name);
    return returned.has_raw() || returned.has_func();
}

auto TypeManager::hasRegisteredType(const Type& name) const noexcept -> bool {
    const auto returned = this->getRegisteredType(name);
    return returned.has_raw() || returned.has_func();
}

auto TypeManager::getRegisteredType(const std::string& name) const noexcept -> Type {
    Type ty;
    ty.mutable_raw()->set_name(name);
    return this->getRegisteredType(ty);
}

auto TypeManager::getRegisteredType(const Type& name) const noexcept -> Type {
	for (auto& type : this->registeredTypes) {
        if (proto_equal(type, name)) {
			return type;
		}
	}

	return {};
}

auto TypeManager::canGetFunctionOverloads(const ConstraintPass::IDType& funcID, const ConstraintPass* pass) const -> bool {
    for (auto& overload : this->functions) {
        if (overload.id() == funcID) {
            // Check if it's `ready`
            for (auto& arg : overload.args()) {
                if (!pass->hasResolvedType(arg)) {
                    return false;
                }
            }
            if (!pass->hasResolvedType(overload.returnvar())) {
                return false;
            }
        }
    }
    return true;
}

auto TypeManager::getFunctionOverloads(const ConstraintPass::IDType& funcID, const ConstraintPass* pass) const -> std::vector<FunctionDefinition> {
    std::vector<FunctionDefinition> overloads;
    for (auto& overload : this->functions) {
        // Lookup by 'var', to deal with anonymous functions.
        if (overload.id() == funcID) {
            // Copy it over, and hand it over a 'function definition'.
            FunctionDefinition funcDef;
            for (auto& arg : overload.args()) {
                funcDef.add_args()->CopyFrom(pass->getResolvedType(arg));
            }
            funcDef.mutable_returntype()->CopyFrom(pass->getResolvedType(overload.returnvar()));

            overloads.push_back(funcDef);
        }
    }

    return overloads;
}

auto TypeManager::setConvertible(const std::string& T0, const std::string& T1) -> bool {
    Type t0;
    t0.mutable_raw()->set_name(T0);

    Type t1;
    t1.mutable_raw()->set_name(T1);

    return this->setConvertible(t0, t1);
}

auto join(const std::string& separator, const std::vector<std::string>& input) -> std::string {
    std::string out;
    if (input.size() > 1) {
        for (std::size_t i = 0; i < input.size() - 1; ++i) {
            out += (input.at(i) + separator);
        }
    }
    if (input.size() > 1) {
        out += input.back();
    }
    return out;
}

auto TypeManager::CreateFunctionHash(const std::string& name, const std::vector<std::string>& argNames) const -> ConstraintPass::IDType {
    return static_cast<ConstraintPass::IDType>(std::hash<std::string>()(name + join(":", argNames)));
}

auto TypeManager::CreateLambdaFunctionHash(const std::vector<std::string>& argNames) const -> ConstraintPass::IDType {
    // Lambda functions use the address of the arguments as part of the name
    const void* address = static_cast<const void*>(&argNames);
    std::stringstream ss;
    ss << address;
    std::string lambdaAddress = ss.str();

    return this->CreateFunctionHash("lambda" + lambdaAddress, argNames);
}

auto TypeManager::setConvertible(const Type& T0, const Type& T1) -> bool {
    if (proto_equal(T0, T1)) {
		return true;
	}

	const auto& t0_ptr = this->getRegisteredType(T0);
	const auto& t1_ptr = this->getRegisteredType(T1);

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

auto TypeManager::getResolvedType(const TypeVar& type) const -> const Type {
	return this->solver.getResolvedType(type);
}

auto TypeManager::isConvertible(const std::string& T0, const std::string& T1) const noexcept -> bool {
    Type t0;
    t0.mutable_raw()->set_name(T0);

    Type t1;
    t1.mutable_raw()->set_name(T1);
    return this->isConvertible(t0, t1);
}

auto TypeManager::isConvertible(const Type& T0, const Type& T1) const noexcept -> bool {
    if (T0.raw().name().empty() || T1.raw().name().empty()) {
        // Undefined types, stop here.
        return false;
    }

    // Function types not convertible
    if (T0.has_func() || T1.has_func()) {
        return false;
    }

    if (proto_equal(T0, T1)) {
		return true;
	}

    // Because they're not functions, they must both be raw.
    if (this->convertible.find(T0.raw().name()) == this->convertible.end()) {
		// T0 is not in the map, meaning the conversion won't be there.
		return false;
	}

    if (this->convertible.at(T0.raw().name()).find(T1.raw().name()) != this->convertible.at(T0.raw().name()).end()) {
		// Convertible from T0 -> T1
		return true;
	}
	return false;
}

auto TypeManager::getConvertible(const Type& T0) const -> std::vector<Type> {
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

auto TypeManager::registerResolver(std::unique_ptr<Resolver>&& resolver) -> bool {
	bool will_insert = this->registeredResolvers.find(resolver->kind) == this->registeredResolvers.end();
	if (will_insert) {
		// We don't have one yet, so add it
		this->registeredResolvers.emplace(std::make_pair(resolver->kind, std::move(resolver)));
	}

	return will_insert;
}

auto TypeManager::CreateTypeVar() -> const TypeVar {
	const auto var = this->type_generator.next();

	this->registeredTypeVars.insert(var);

	TypeVar type;
	type.set_symbol(var);
	return type;
}

auto TypeManager::getConstraintInternal(const ConstraintPass::IDType id) -> Constraint* {
    for (auto& constraint : this->constraints) {
        if (constraint.id() == id) {
            return &constraint;
        }
    }

    return nullptr;
}

auto TypeManager::getConstraint(const ConstraintPass::IDType id) const -> const Constraint* {
	for (auto& constraint : this->constraints) {
		if (constraint.id() == id) {
			return &constraint;
		}
	}

	return nullptr;
}

auto TypeManager::solve() -> bool {
	// Add default `resolvers`, ignore response
	// it will not double-register, so this is safe
	constexpr auto default_id = std::numeric_limits<std::size_t>::max();
	this->registerResolver(std::make_unique<ResolveConformsTo>(nullptr, default_id));
	this->registerResolver(std::make_unique<ResolveEquals>(nullptr, default_id));
    this->registerResolver(std::make_unique<ResolveConvertible>(nullptr, default_id));
    this->registerResolver(std::make_unique<ResolveApplicableFunction>(nullptr, default_id));
    this->registerResolver(std::make_unique<ResolveBindOverload>(nullptr, default_id));
    this->registerResolver(std::make_unique<ResolveBindTo>(nullptr, default_id));

    this->SortConstraints();

	// Finally, solve
	return this->solver.solve(this);
}
