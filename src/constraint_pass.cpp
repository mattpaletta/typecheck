#include "typecheck/constraint_pass.hpp"

#include "typecheck/type_var.hpp"  // for Constraint, ConstraintKind
#include "typecheck/type.hpp"

#include <iostream>

using namespace typecheck;

auto ConstraintPass::getResolvedType(const TypeVar& var) const -> Type {
	Type type;
    if (!this->hasResolvedType(var)) {
        std::cout << "Typecheck Error: asking for unresolved type: " << var.symbol() << std::endl;
        return type;
    }

    return this->resolvedTypes.at(var.symbol());
}

auto ConstraintPass::hasResolvedType(const TypeVar& var) const -> bool {
    return this->resolvedTypes.find(var.symbol()) != this->resolvedTypes.end();
}

auto ConstraintPass::setResolvedType(const TypeVar& var, const Type& type) -> bool {
    if (!var.symbol().empty()) {
        this->resolvedTypes[var.symbol()] = type;
        return true;
    }

    return false;
}
