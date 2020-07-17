//
//  resolver.cpp
//  typecheck
//
//  Created by Matthew Paletta on 2020-07-17.
//

#include "typecheck/resolver.hpp"

typecheck::Resolver::Resolver(ConstraintKind _kind, ConstraintPass* _pass, const ConstraintPass::ConstraintIDType _id) : pass(_pass), kind(_kind), id(_id) {}

std::unique_ptr<typecheck::Resolver> typecheck::Resolver::clone(ConstraintPass* _pass, const ConstraintPass::ConstraintIDType _id) const {
    return std::make_unique<Resolver>(this->kind, _pass, _id);
}

bool typecheck::Resolver::hasMoreSolutions([[maybe_unused]] const Constraint& constraint, [[maybe_unused]] const TypeManager* manager) {
    // Setup method
    return false;
}

bool typecheck::Resolver::resolveNext([[maybe_unused]] const Constraint& constraint, [[maybe_unused]] const TypeManager* manager) {
    // Called subsequent times.
    return false;
}

std::size_t typecheck::Resolver::score([[maybe_unused]] const Constraint& constraint, [[maybe_unused]] const TypeManager* manager) const {
    return 0;
}
