//
//  ResolveApplicableFunction.cpp
//  typecheck
//
//  Created by Matthew Paletta on 2020-07-17.
//

#include "typecheck/resolvers/ResolveApplicableFunction.hpp"
#include "typecheck/debug.hpp"

using namespace typecheck;

ResolveApplicableFunction::ResolveApplicableFunction(ConstraintPass* _pass, const ConstraintPass::IDType _id) : Resolver(ConstraintKind::ApplicableFunction, _pass, _id) {}

auto ResolveApplicableFunction::clone(ConstraintPass* _pass, const ConstraintPass::IDType _id) const -> std::unique_ptr<Resolver> {
    return std::make_unique<ResolveApplicableFunction>(_pass, _id);
}

auto ResolveApplicableFunction::is_valid_constraint(const Constraint& constraint) const -> bool {
    return constraint.has_explicit_() && constraint.explicit_().has_var() && constraint.explicit_().has_type() && constraint.explicit_().type().has_func();
}

auto ResolveApplicableFunction::hasMoreSolutions(const Constraint& constraint, [[maybe_unused]] const TypeManager* manager) -> bool {
    return this->pass && !has_gotten_resolve && this->is_valid_constraint(constraint);
}

auto ResolveApplicableFunction::resolveNext(const Constraint& constraint, [[maybe_unused]] const TypeManager* manager) -> bool {
    this->has_gotten_resolve = true;

    // See bindOverload for selecting the correct implementation

    // Don't have a pass, can't resolve.
    return this->is_valid_constraint(constraint);
}

auto ResolveApplicableFunction::score(const Constraint& constraint, [[maybe_unused]] const TypeManager* manager) const -> std::size_t {
    if (!this->is_valid_constraint(constraint)) {
        return std::numeric_limits<std::size_t>::max();
    }

    // This is pretty simple, used to define functions, so it passes!

    return 0;
}
