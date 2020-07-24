//
//  resolver.cpp
//  typecheck
//
//  Created by Matthew Paletta on 2020-07-17.
//

#include "typecheck/resolver.hpp"

typecheck::Resolver::Resolver(ConstraintKind _kind, ConstraintPass* _pass, const ConstraintPass::IDType _id) : pass(_pass), kind(_kind), id(_id) {}

// LCOV_EXCL_START
auto typecheck::Resolver::clone(ConstraintPass* _pass, const ConstraintPass::IDType _id) const -> std::unique_ptr<typecheck::Resolver> {
    return std::make_unique<Resolver>(this->kind, _pass, _id);
}

auto typecheck::Resolver::hasMoreSolutions([[maybe_unused]] const Constraint& constraint, [[maybe_unused]] const TypeManager* manager) -> bool {
    // Setup method
    return false;
}

auto typecheck::Resolver::resolveNext([[maybe_unused]] const Constraint& constraint, [[maybe_unused]] const TypeManager* manager) -> bool {
    // Called subsequent times.
    return false;
}

auto typecheck::Resolver::score([[maybe_unused]] const Constraint& constraint, [[maybe_unused]] const TypeManager* manager) const -> std::size_t {
    return 0;
}

// LCOV_EXCL_STOP
