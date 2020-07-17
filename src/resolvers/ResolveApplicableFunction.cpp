//
//  ResolveApplicableFunction.cpp
//  typecheck
//
//  Created by Matthew Paletta on 2020-07-17.
//

#include "typecheck/resolvers/ResolveApplicableFunction.hpp"
#include "typecheck/debug.hpp"

#include <google/protobuf/util/message_differencer.h>

typecheck::ResolveApplicableFunction::ResolveApplicableFunction(ConstraintPass* pass, const ConstraintPass::ConstraintIDType id) : Resolver(ConstraintKind::ApplicableFunction, pass, id) {}

std::unique_ptr<typecheck::Resolver> typecheck::ResolveApplicableFunction::clone(ConstraintPass* pass, const ConstraintPass::ConstraintIDType id) const {
    return std::make_unique<ResolveApplicableFunction>(pass, id);
}

bool typecheck::ResolveApplicableFunction::is_valid_constraint(const Constraint& constraint) const {
    return constraint.has_explicit_() && constraint.explicit_().has_var() && constraint.explicit_().has_type() && constraint.explicit_().type().has_func();
}

bool typecheck::ResolveApplicableFunction::hasMoreSolutions(const Constraint& constraint, [[maybe_unused]] const TypeManager* manager) {
    return this->pass && !has_gotten_resolve && this->is_valid_constraint(constraint);
}

bool typecheck::ResolveApplicableFunction::resolveNext(const Constraint& constraint, [[maybe_unused]] const TypeManager* manager) {
    this->has_gotten_resolve = true;

    // See bindOverload for selecting the correct implementation

    // Don't have a pass, can't resolve.
    return this->is_valid_constraint(constraint);
}

std::size_t typecheck::ResolveApplicableFunction::score(const Constraint& constraint, [[maybe_unused]] const TypeManager* manager) const {
    if (!this->is_valid_constraint(constraint)) {
        return std::numeric_limits<std::size_t>::max();
    }

    // This is pretty simple, used to define functions, so it passes!

    return 0;
}