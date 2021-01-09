//
//  ResolveBindTo.cpp
//  typecheck
//
//  Created by Matthew Paletta on 2020-07-17.
//

#include "typecheck/resolvers/ResolveBindTo.hpp"
#include "typecheck/debug.hpp"

using namespace typecheck;

ResolveBindTo::ResolveBindTo(const ConstraintPass::IDType _id) : Resolver(ConstraintKind::Bind, _id) {}

auto ResolveBindTo::clone(const ConstraintPass::IDType _id) const -> std::unique_ptr<Resolver> {
    return std::make_unique<ResolveBindTo>(_id);
}

auto ResolveBindTo::is_valid_constraint(const Constraint& constraint, ConstraintPass* pass) const -> bool {
    return constraint.has_explicit_() && constraint.explicit_().has_type() && constraint.explicit_().has_var();
}

auto ResolveBindTo::hasMoreSolutions(const Constraint& constraint, ConstraintPass* pass, [[maybe_unused]] const TypeManager* manager) -> bool {
    return !this->has_gotten_resolve && this->is_valid_constraint(constraint, pass);
}

auto ResolveBindTo::resolveNext(const Constraint& constraint, ConstraintPass* pass, const TypeManager* manager) -> bool {
    this->has_gotten_resolve = true;
    if (!this->is_valid_constraint(constraint, pass)) {
        return false;
    }

    const auto& var = constraint.explicit_().var();
    const auto& type = constraint.explicit_().type();
    if (pass->hasResolvedType(var)) {
        // It's already been resolved, but it's the same, so we're happy
        if (proto_equal(pass->getResolvedType(var), type)) {
            return true;
        }
    }

    // it's either not assigned, or it's not the same, so try and modify it.
    return pass->setResolvedType(constraint, var, type, manager);
}

auto ResolveBindTo::score(const Constraint& constraint, ConstraintPass* pass, [[maybe_unused]] const TypeManager* manager) const -> std::size_t {
    if (!this->is_valid_constraint(constraint, pass) || !pass->hasResolvedType(constraint.explicit_().var())) {
        return std::numeric_limits<std::size_t>::max();
    }

    // If var is resolved, and it's type equals what it's supposed to be.
    if (proto_equal(pass->getResolvedType(constraint.explicit_().var()), constraint.explicit_().type())) {
        // All args found, and matched up, and return types found and match up.
        return 0;
    }

    return std::numeric_limits<std::size_t>::max();
}
