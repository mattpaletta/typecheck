//
//  resolver.cpp
//  typecheck
//
//  Created by Matthew Paletta on 2020-07-17.
//

#include "typecheck/resolver.hpp"

using namespace typecheck;

Resolver::Resolver(ConstraintKind _kind, const ConstraintPass::IDType _id) : kind(_kind), id(_id) {}

// LCOV_EXCL_START
auto Resolver::clone(const ConstraintPass::IDType _id) const -> std::unique_ptr<Resolver> {
    return std::make_unique<Resolver>(this->kind, _id);
}

auto Resolver::hasMoreSolutions([[maybe_unused]] const Constraint& constraint, ConstraintPass* pass, [[maybe_unused]] const TypeManager* manager) -> bool {
    // Setup method
    return false;
}

auto Resolver::readyToResolve([[maybe_unused]] const Constraint& constraint, ConstraintPass* pass, [[maybe_unused]] const TypeManager* manager) const -> bool {
    return true;
}

auto Resolver::resolveNext([[maybe_unused]] const Constraint& constraint, ConstraintPass* pass, [[maybe_unused]] const TypeManager* manager) -> bool {
    // Called subsequent times.
    return false;
}

auto Resolver::score([[maybe_unused]] const Constraint& constraint, ConstraintPass* pass, [[maybe_unused]] const TypeManager* manager) const -> std::size_t {
    return 0;
}

// LCOV_EXCL_STOP
